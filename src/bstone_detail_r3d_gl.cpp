/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2020 Boris I. Bendovsky (bibendovsky@hotmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


//
// OpenGL 3D renderer (implementation).
//
// !!! Internal usage only !!!
//



#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl.h"

#include <cassert>

#include "bstone_exception.h"
#include "bstone_r3d_cmd_queue.h"
#include "bstone_r3d_limits.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_buffer_mgr.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_sampler_mgr.h"
#include "bstone_detail_r3d_gl_texture_mgr.h"
#include "bstone_detail_r3d_gl_vertex_input_mgr.h"
#include "bstone_detail_r3d_gl_shader_mgr.h"
#include "bstone_detail_r3d_gl_shader_stage_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGl
//

R3dGl::R3dGl(
	const R3dCreateParam& param)
	:
	kind_{},
	name_{},
	description_{},
	device_info_{},
	device_features_{},
	gl_device_features_{},
	screen_width_{},
	screen_height_{},
	aa_kind_{},
	aa_value_{},
	sdl_window_{},
	sdl_gl_context_{},
	extension_manager_{},
	gl_context_{},
	gl_msaa_fbo_{},
	gl_msaa_color_rb_{},
	gl_msaa_depth_rb_{}
{
	switch (param.renderer_kind_)
	{
		case R3dKind::gl_2_0:
		case R3dKind::gl_3_2_core:
		case R3dKind::gles_2_0:
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	kind_ = param.renderer_kind_;

	R3dGlUtils::msaa_probe(kind_, device_features_, gl_device_features_);

	aa_kind_ = param.aa_kind_;
	aa_value_ = param.aa_value_;

	auto window_param = R3dUtilsCreateWindowParam{};
	window_param.renderer_kind_ = kind_;
	window_param.window_ = param.window_;
	window_param.aa_kind_ = aa_kind_;
	window_param.aa_value_ = aa_value_;

	if (window_param.window_.width_ == 0 || window_param.window_.height_ == 0)
	{
		window_param.window_.width_ = 1;
		window_param.window_.height_ = 1;
	}

	if (window_param.aa_kind_ == R3dAaKind::ms)
	{
		if (device_features_.msaa_is_available_)
		{
			if (aa_value_ <= 0)
			{
				aa_value_ = device_features_.msaa_max_degree_;
			}

			if (aa_value_ < R3dLimits::aa_min_off)
			{
				aa_value_ = R3dLimits::aa_min_off;
			}

			if (aa_value_ > device_features_.msaa_max_degree_)
			{
				aa_value_ = device_features_.msaa_max_degree_;
			}

			if (device_features_.msaa_is_render_to_window_)
			{
				window_param.aa_value_ = aa_value_;
			}
			else
			{
				window_param.aa_kind_ = R3dAaKind::none;
				window_param.aa_value_ = 0;
				window_param.is_default_depth_buffer_disabled_ = true;
			}
		}
		else
		{
			window_param.aa_kind_ = R3dAaKind::none;
			window_param.aa_value_ = 0;
		}
	}

	R3dGlUtils::create_window_and_context(window_param, sdl_window_, sdl_gl_context_);

	R3dGlUtils::window_get_drawable_size(
		sdl_window_.get(),
		screen_width_,
		screen_height_);

	if (screen_width_ == 0 || screen_height_ == 0)
	{
		throw Exception{"Failed to get screen size."};
	}

	if (aa_kind_ == R3dAaKind::ms && device_features_.msaa_is_render_to_window_)
	{
		aa_value_ = R3dGlUtils::msaa_window_get_value();
	}

	extension_manager_ = detail::R3dGlExtensionMgrFactory::create();

	if (extension_manager_ == nullptr)
	{
		throw Exception{"Failed to create an extension manager."};
	}

	switch (kind_)
	{
		case R3dKind::gl_2_0:
			extension_manager_->probe(R3dGlExtensionId::v2_0);

			if (!extension_manager_->has(R3dGlExtensionId::v2_0))
			{
				throw Exception{"Failed to load OpenGL 2.0 symbols."};
			}

			break;

		case R3dKind::gl_3_2_core:
			extension_manager_->probe(R3dGlExtensionId::v3_2_core);

			if (!extension_manager_->has(R3dGlExtensionId::v3_2_core))
			{
				throw Exception{"Failed to load OpenGL 3.2 core symbols."};
			}

			break;

		case R3dKind::gles_2_0:
			extension_manager_->probe(R3dGlExtensionId::es_v2_0);

			if (!extension_manager_->has(R3dGlExtensionId::es_v2_0))
			{
				throw Exception{"Failed to load OpenGL ES 2.0 symbols."};
			}

			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	R3dGlUtils::renderer_features_set(device_features_);

	gl_device_features_.context_kind_ = R3dGlUtils::context_get_kind();

	R3dGlUtils::anisotropy_probe(
		extension_manager_.get(),
		device_features_
	);

	R3dGlUtils::npot_probe(
		extension_manager_.get(),
		device_features_
	);

	R3dGlUtils::mipmap_probe(
		extension_manager_.get(),
		device_features_,
		gl_device_features_
	);

	R3dGlUtils::framebuffer_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	R3dGlUtils::sampler_probe(
		extension_manager_.get(),
		device_features_
	);

	R3dGlUtils::vertex_input_probe_max_locations(device_features_);

	R3dGlUtils::buffer_storage_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	R3dGlUtils::dsa_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	R3dGlUtils::sso_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	if (device_features_.vertex_input_max_locations_ <= 0)
	{
		throw Exception{"No vertex input locations."};
	}

	R3dGlUtils::vsync_probe(device_features_);

	R3dGlUtils::vertex_input_vao_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	gl_context_ = R3dGlContextFactory::create(
		kind_,
		device_features_,
		gl_device_features_
	);

	if (device_features_.vsync_is_available_)
	{
		static_cast<void>(R3dGlUtils::vsync_set(param.is_vsync_));
	}

	framebuffers_create();

	device_info_ = R3dGlUtils::device_info_get();

	set_name_and_description();


	// Present.
	//
	gl_context_->clear(Rgba8{});
	present();
}

R3dGl::~R3dGl() = default;

R3dKind R3dGl::get_kind() const noexcept
{
	return kind_;
}

const std::string& R3dGl::get_name() const noexcept
{
	return name_;
}

const std::string& R3dGl::get_description() const noexcept
{
	return description_;
}

void R3dGl::fbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteFramebuffers ? glDeleteFramebuffers : glDeleteFramebuffersEXT);
	gl_function(1, &gl_name);
	R3dGlError::ensure_debug();
}

void R3dGl::rbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteRenderbuffers ? glDeleteRenderbuffers : glDeleteRenderbuffersEXT);
	gl_function(1, &gl_name);
	R3dGlError::ensure_debug();
}

void R3dGl::set_name_and_description()
{
	switch (kind_)
	{
		case R3dKind::gl_2_0:
			name_ = "GL2";
			description_ = "OpenGL 2.0+";
			break;

		case R3dKind::gl_3_2_core:
			name_ = "GL3.2C";
			description_ = "OpenGL 3.2 core";
			break;

		case R3dKind::gles_2_0:
			name_ = "GLES2.0";
			description_ = "OpenGL ES 2.0";
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

const R3dDeviceFeatures& R3dGl::device_get_features() const noexcept
{
	return device_features_;
}

const R3dDeviceInfo& R3dGl::device_get_info() const noexcept
{
	return device_info_;
}

void R3dGl::window_set_mode(
	const R3dWindowSetModeParam& param)
{
	R3dUtils::window_set_mode(sdl_window_.get(), param);

	const auto size_changed = (screen_width_ != param.width_ || screen_height_ != param.height_);

	screen_width_ = param.width_;
	screen_height_ = param.height_;

	if (size_changed && gl_device_features_.framebuffer_is_available_)
	{
		msaa_framebuffer_destroy();
		msaa_framebuffer_create();
	}
}

void R3dGl::window_set_title(
	const std::string& title_utf8)
{
	R3dUtils::window_set_title(sdl_window_.get(), title_utf8);
}

void R3dGl::window_show(
	const bool is_visible)
{
	R3dUtils::window_show(sdl_window_.get(), is_visible);
}

bool R3dGl::vsync_get() const noexcept
{
	if (!device_features_.vsync_is_available_)
	{
		return false;
	}

	return R3dGlUtils::vsync_get();
}

void R3dGl::vsync_set(
	const bool is_enabled)
{
	if (!device_features_.vsync_is_available_)
	{
		throw Exception{"Not available."};
	}

	if (device_features_.vsync_is_requires_restart_)
	{
		throw Exception{"Requires restart."};
	}

	if (!R3dGlUtils::vsync_set(is_enabled))
	{
		throw Exception{"Not supported."};
	}
}

void R3dGl::aa_set(
	const R3dAaKind aa_kind,
	const int aa_value)
{
	switch (aa_kind)
	{
		case R3dAaKind::none:
		case R3dAaKind::ms:
			break;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}

	auto clamped_aa_value = aa_value;

	if (clamped_aa_value < R3dLimits::aa_min_off)
	{
		clamped_aa_value = R3dLimits::aa_min_off;
	}

	if (clamped_aa_value > R3dLimits::aa_max)
	{
		clamped_aa_value = R3dLimits::aa_max;
	}

	switch (aa_kind)
	{
		case R3dAaKind::none:
			aa_disable();
			return;

		case R3dAaKind::ms:
			msaa_set(clamped_aa_value);
			return;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}
}

void R3dGl::present()
{
	framebuffers_blit();

	R3dGlError::ensure();

	R3dGlUtils::swap_window(sdl_window_.get());

	framebuffers_bind();
}

R3dBufferUPtr R3dGl::buffer_create(
	const R3dBufferCreateParam& param)
{
	return gl_context_->buffer_get_manager()->buffer_create(param);
}

R3dVertexInputUPtr R3dGl::vertex_input_create(
	const R3dVertexInputCreateParam& param)
{
	return gl_context_->vertex_input_get_manager()->create(param);
}

R3dShaderUPtr R3dGl::shader_create(
	const R3dShaderCreateParam& param)
{
	return gl_context_->shader_get_manager()->create(param);
}

R3dShaderStageUPtr R3dGl::shader_stage_create(
	const R3dShaderStageCreateParam& param)
{
	return gl_context_->shader_stage_get_manager()->create(param);
}

R3dTexture2dUPtr R3dGl::texture_2d_create(
	const R3dTexture2dCreateParam& param)
{
	return gl_context_->texture_get_manager()->create(param);
}

R3dSamplerUPtr R3dGl::sampler_create(
	const R3dSamplerCreateParam& param)
{
	return gl_context_->sampler_get_manager()->create(param);
}

R3dGl::RboResource R3dGl::renderbuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_function = (gl_device_features_.framebuffer_is_ext_ ? glGenRenderbuffersEXT : glGenRenderbuffers);

	auto gl_name = GLuint{};
	gl_function(1, &gl_name);
	R3dGlError::ensure_debug();

	auto rbo_resource = RboResource{gl_name};

	if (!rbo_resource)
	{
		throw Exception{"Failed to create OpenGL renderbuffer object."};
	}

	return rbo_resource;
}

void R3dGl::renderbuffer_bind(
	const GLuint gl_renderbuffer_name)
{
	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindRenderbufferEXT : glBindRenderbuffer);

	gl_func(GL_RENDERBUFFER, gl_renderbuffer_name);
	R3dGlError::ensure_debug();
}

R3dGl::FboResource R3dGl::framebuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glGenFramebuffersEXT : glGenFramebuffers);

	auto gl_name = GLuint{};
	gl_func(1, &gl_name);
	R3dGlError::ensure_debug();

	auto fbo_resource = FboResource{gl_name};

	if (!fbo_resource)
	{
		throw Exception{"Failed to create OpenGL framebuffer object."};
	}

	return fbo_resource;
}

void R3dGl::framebuffer_bind(
	const GLenum gl_target,
	const GLuint gl_name)
{
	assert(gl_device_features_.framebuffer_is_available_);

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindFramebufferEXT : glBindFramebuffer);

	gl_func(gl_target, gl_name);
	R3dGlError::ensure_debug();
}

void R3dGl::framebuffer_blit(
	const int src_width,
	const int src_height,
	const int dst_width,
	const int dst_height,
	const bool is_linear_filter)
{
	assert(src_width > 0);
	assert(src_height > 0);
	assert(dst_width > 0);
	assert(dst_height > 0);

	assert(gl_device_features_.framebuffer_is_available_);

	const auto gl_func = (
		gl_device_features_.framebuffer_is_ext_ ?
		glBlitFramebufferEXT :
		glBlitFramebuffer
	);

	const auto gl_filter = (is_linear_filter ? GL_LINEAR : GL_NEAREST);

	gl_func(
		0,
		0,
		src_width,
		src_height,
		0,
		0,
		dst_width,
		dst_height,
		GL_COLOR_BUFFER_BIT,
		gl_filter
	);

	R3dGlError::ensure_debug();
}

R3dGl::RboResource R3dGl::renderbuffer_create(
	const int width,
	const int height,
	const int sample_count,
	const GLenum gl_internal_format)
{
	assert(width > 0);
	assert(height > 0);
	assert(sample_count >= 0);
	assert(gl_internal_format > 0);

	auto rbo_resource = renderbuffer_create();
	renderbuffer_bind(rbo_resource.get());

	assert(gl_device_features_.framebuffer_is_available_);

	const auto gl_func = (
		gl_device_features_.framebuffer_is_ext_ ?
		glRenderbufferStorageMultisampleEXT :
		glRenderbufferStorageMultisample
	);

	gl_func(GL_RENDERBUFFER, sample_count, gl_internal_format, width, height);
	R3dGlError::ensure_debug();

	renderbuffer_bind(0);

	return rbo_resource;
}

void R3dGl::msaa_color_rb_destroy()
{
	gl_msaa_color_rb_.reset();
}

void R3dGl::msaa_depth_rb_destroy()
{
	gl_msaa_depth_rb_.reset();
}

void R3dGl::msaa_fbo_destroy()
{
	gl_msaa_fbo_.reset();
}

void R3dGl::msaa_framebuffer_destroy()
{
	msaa_fbo_destroy();
	msaa_color_rb_destroy();
	msaa_depth_rb_destroy();
}

void R3dGl::msaa_color_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_color_rb_ = renderbuffer_create(width, height, sample_count, GL_RGBA8);
}

void R3dGl::msaa_depth_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_depth_rb_ = renderbuffer_create(width, height, sample_count, GL_DEPTH_COMPONENT);
}

void R3dGl::msaa_framebuffer_create()
{
	auto aa_degree = aa_value_;

	if (aa_kind_ == R3dAaKind::none)
	{
		aa_degree = R3dLimits::aa_min_off;
	}

	if (aa_degree < R3dLimits::aa_min_on)
	{
		aa_degree = R3dLimits::aa_min_off;
	}

	if (aa_degree > device_features_.msaa_max_degree_)
	{
		aa_degree = device_features_.msaa_max_degree_;
	}

	msaa_color_rb_create(screen_width_, screen_height_, aa_degree);
	msaa_depth_rb_create(screen_width_, screen_height_, aa_degree);

	gl_msaa_fbo_ = framebuffer_create();
	framebuffer_bind(GL_FRAMEBUFFER, gl_msaa_fbo_);

	const auto framebuffer_renderbuffer_func = (
		gl_device_features_.framebuffer_is_ext_ ?
		glFramebufferRenderbufferEXT :
		glFramebufferRenderbuffer
	);

	framebuffer_renderbuffer_func(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER,
		gl_msaa_color_rb_
	);

	framebuffer_renderbuffer_func(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		gl_msaa_depth_rb_
	);

	const auto check_framebuffer_status_func = (
		gl_device_features_.framebuffer_is_ext_ ?
		glCheckFramebufferStatusEXT :
		glCheckFramebufferStatus
	);

	const auto framebuffer_status = check_framebuffer_status_func(GL_FRAMEBUFFER);

	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw Exception{"Incomplete framebuffer object."};
	}

	framebuffer_bind(GL_FRAMEBUFFER, 0);
}

void R3dGl::framebuffers_destroy()
{
	msaa_framebuffer_destroy();
}

void R3dGl::framebuffers_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		return;
	}

	msaa_framebuffer_create();
}

void R3dGl::framebuffers_blit()
{
	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	// MSAA FBO -> Default FBO
	//

	// Read: MSAA
	// Draw: Default
	framebuffer_bind(GL_DRAW_FRAMEBUFFER, 0);

	framebuffer_blit(
		screen_width_,
		screen_height_,
		screen_width_,
		screen_height_,
		false
	);
}

void R3dGl::framebuffers_bind()
{
	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	framebuffer_bind(GL_FRAMEBUFFER, gl_msaa_fbo_);
}

void R3dGl::aa_disable()
{
	aa_kind_ = R3dAaKind::none;

	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	msaa_framebuffer_destroy();
	msaa_framebuffer_create();
}

void R3dGl::msaa_set(
	const int aa_value)
{
	if (device_features_.msaa_is_requires_restart_)
	{
		throw Exception{"Requires restart."};
	}

	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	if (aa_kind_ == R3dAaKind::ms && aa_value_ == aa_value)
	{
		return;
	}

	aa_kind_ = R3dAaKind::ms;
	aa_value_ = aa_value;

	framebuffers_destroy();

	framebuffers_create();
}

void R3dGl::command_execute_clear(
	const R3dCmdClear& command)
{
	gl_context_->clear(command.param_.color_);
}

void R3dGl::command_execute_culling(
	const R3dCmdCulling& command)
{
	gl_context_->culling_enable(command.is_enable_);
}

void R3dGl::command_execute_depth_test(
	const R3dCmdDepthTest& command)
{
	gl_context_->depth_test_enable(command.is_enable_);
}

void R3dGl::command_execute_depth_write(
	const R3dCmdDepthWrite& command)
{
	gl_context_->depth_write_enable(command.is_enable_);
}

void R3dGl::command_execute_viewport(
	const R3dCmdViewport& command)
{
	gl_context_->viewport_set(command.viewport_);
}

void R3dGl::command_execute_blending(
	const R3dCmdBlending& command)
{
	gl_context_->blending_enable(command.is_enable_);
}

void R3dGl::command_execute_blending_func(
	const R3dCmdBlendingFunc& command)
{
	gl_context_->blending_set_func(command.blending_func_);
}

void R3dGl::command_execute_scissor(
	const R3dCmdScissor& command)
{
	gl_context_->scissor_enable(command.is_enable_);
}

void R3dGl::command_execute_scissor_box(
	const R3dCmdScissorBox& command)
{
	gl_context_->scissor_set_box(command.scissor_box_);
}

void R3dGl::command_execute_texture(
	const R3dCmdTexture& command)
{
	gl_context_->texture_get_manager()->set_current(command.texture_2d_);
	gl_context_->texture_get_manager()->set(command.texture_2d_);
}

void R3dGl::command_execute_sampler(
	const R3dCmdSampler& command)
{
	gl_context_->sampler_get_manager()->set(command.sampler_);
}

void R3dGl::command_execute_vertex_input(
	const R3dCmdVertexInput& command)
{
	gl_context_->vertex_input_get_manager()->set(command.vertex_input_);
}

void R3dGl::command_execute_shader_stage(
	const R3dCmdShaderStage& command)
{
	gl_context_->shader_stage_get_manager()->set_current(command.shader_stage_);
	gl_context_->shader_stage_get_manager()->set(command.shader_stage_);
}

void R3dGl::command_execute_shader_var_int32(
	const R3dCmdShaderVarInt32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_int(command.value_);
}

void R3dGl::command_execute_shader_var_float32(
	const R3dCmdShaderVarFloat32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_float(command.value_);
}

void R3dGl::command_execute_shader_var_vec2(
	const R3dCmdShaderVarVec2& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec2(command.value_.data());
}

void R3dGl::command_execute_shader_var_vec4(
	const R3dCmdShaderVarVec4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec4(command.value_.data());
}

void R3dGl::command_execute_shader_var_mat4(
	const R3dCmdShaderVarMat4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_mat4(command.value_.data());
}

void R3dGl::command_execute_shader_var_sampler_2d(
	const R3dCmdShaderVarSampler2d& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_sampler_2d(command.value_);
}

void R3dGl::command_execute_draw_indexed(
	const R3dCmdDrawIndexed& command)
{
	const auto& param = command.param_;

	auto gl_primitive_topology = GLenum{};

	switch (param.primitive_topology_)
	{
		case R3dPrimitiveTopology::point_list:
			gl_primitive_topology = GL_POINTS;
			break;

		case R3dPrimitiveTopology::line_list:
			gl_primitive_topology = GL_LINES;
			break;

		case R3dPrimitiveTopology::line_strip:
			gl_primitive_topology = GL_LINE_STRIP;
			break;

		case R3dPrimitiveTopology::triangle_list:
			gl_primitive_topology = GL_TRIANGLES;
			break;

		case R3dPrimitiveTopology::triangle_strip:
			gl_primitive_topology = GL_TRIANGLE_STRIP;
			break;

		default:
			throw Exception{"Unsupported primitive topology."};
	}

	if (param.vertex_count_ < 0)
	{
		throw Exception{"Vertex count out of range."};
	}

	if (param.vertex_count_ == 0)
	{
		return;
	}

	switch (param.index_byte_depth_)
	{
		case 1:
		case 2:
		case 4:
			break;

		default:
			throw Exception{"Unsupported index value byte depth."};
	}

	if (param.index_buffer_offset_ < 0)
	{
		throw Exception{"Offset to indices out of range."};
	}

	if (param.index_offset_ < 0)
	{
		throw Exception{"Index offset out of range."};
	}

	auto index_buffer = static_cast<R3dGlBufferPtr>(gl_context_->vertex_input_get_manager()->get_current_index_buffer());

	if (!index_buffer)
	{
		throw Exception{"Null index buffer."};
	}

	gl_context_->shader_stage_get_manager()->set_to_current();
	gl_context_->texture_get_manager()->set_to_current();

	const auto index_buffer_offset = param.index_buffer_offset_ + (param.index_offset_ * param.index_byte_depth_);
	const auto index_buffer_indices = reinterpret_cast<const void*>(static_cast<std::intptr_t>(index_buffer_offset));

	const auto gl_element_type = R3dGlUtils::index_buffer_get_element_type_by_byte_depth(
		param.index_byte_depth_);

	index_buffer->set(true);

	glDrawElements(
		gl_primitive_topology, // mode
		param.vertex_count_, // count
		gl_element_type, // type
		index_buffer_indices // indices
	);

	R3dGlError::ensure_debug();
}

void R3dGl::submit_commands(
	const R3dCmdQueuePtr command_queue)
{
	const auto buffer_count = command_queue->get_count();

	for (int i = 0; i < buffer_count; ++i)
	{
		auto command_buffer = command_queue->get(i);

		if (!command_buffer->is_enabled())
		{
			continue;
		}

		const auto command_count = command_buffer->get_command_count();

		command_buffer->read_begin();

		for (int j = 0; j < command_count; ++j)
		{
			const auto command_id = command_buffer->read_command_id();

			switch (command_id)
			{
			case R3dCmdId::clear:
				command_execute_clear(*command_buffer->read_clear());
				break;

			case R3dCmdId::culling:
				command_execute_culling(*command_buffer->read_culling());
				break;

			case R3dCmdId::depth_set_test:
				command_execute_depth_test(*command_buffer->read_depth_test());
				break;

			case R3dCmdId::depth_set_write:
				command_execute_depth_write(*command_buffer->read_depth_write());
				break;

			case R3dCmdId::viewport:
				command_execute_viewport(*command_buffer->read_viewport());
				break;

			case R3dCmdId::scissor:
				command_execute_scissor(*command_buffer->read_scissor());
				break;

			case R3dCmdId::scissor_set_box:
				command_execute_scissor_box(*command_buffer->read_scissor_box());
				break;

			case R3dCmdId::blending:
				command_execute_blending(*command_buffer->read_blending());
				break;

			case R3dCmdId::blending_func:
				command_execute_blending_func(*command_buffer->read_blending_func());
				break;

			case R3dCmdId::texture:
				command_execute_texture(*command_buffer->read_texture());
				break;

			case R3dCmdId::sampler:
				command_execute_sampler(*command_buffer->read_sampler());
				break;

			case R3dCmdId::vertex_input:
				command_execute_vertex_input(*command_buffer->read_vertex_input());
				break;

			case R3dCmdId::shader_stage:
				command_execute_shader_stage(*command_buffer->read_shader_stage());
				break;

			case R3dCmdId::shader_var_int32:
				command_execute_shader_var_int32(*command_buffer->read_shader_var_int32());
				break;

			case R3dCmdId::shader_var_float32:
				command_execute_shader_var_float32(*command_buffer->read_shader_var_float32());
				break;

			case R3dCmdId::shader_var_vec2:
				command_execute_shader_var_vec2(*command_buffer->read_shader_var_vec2());
				break;

			case R3dCmdId::shader_var_vec4:
				command_execute_shader_var_vec4(*command_buffer->read_shader_var_vec4());
				break;

			case R3dCmdId::shader_var_mat4:
				command_execute_shader_var_mat4(*command_buffer->read_shader_var_mat4());
				break;

			case R3dCmdId::shader_var_sampler_2d:
				command_execute_shader_var_sampler_2d(*command_buffer->read_shader_var_sampler_2d());
				break;

			case R3dCmdId::draw_indexed:
				command_execute_draw_indexed(*command_buffer->read_draw_indexed());
				break;

			default:
				throw Exception{"Unsupported command id."};
			}
		}

		command_buffer->read_end();
	}
}

//
// R3dGl
// ==========================================================================


} // detail
} // bstone
