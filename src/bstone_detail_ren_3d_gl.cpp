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
#include "bstone_detail_ren_3d_gl.h"

#include <cassert>

#include "bstone_exception.h"
#include "bstone_ren_3d_cmd_queue.h"
#include "bstone_ren_3d_limits.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_buffer_mgr.h"
#include "bstone_detail_ren_3d_gl_error.h"
#include "bstone_detail_ren_3d_gl_utils.h"
#include "bstone_detail_ren_3d_gl_sampler_mgr.h"
#include "bstone_detail_ren_3d_gl_texture_mgr.h"
#include "bstone_detail_ren_3d_gl_vertex_input_mgr.h"
#include "bstone_detail_ren_3d_gl_shader_mgr.h"
#include "bstone_detail_ren_3d_gl_shader_stage_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGl
//

Ren3dGl::Ren3dGl(
	const Ren3dCreateParam& param)
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
		case Ren3dKind::gl_2_0:
		case Ren3dKind::gl_3_2_core:
		case Ren3dKind::gles_2_0:
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	kind_ = param.renderer_kind_;

	Ren3dGlUtils::msaa_probe(kind_, device_features_, gl_device_features_);

	aa_kind_ = param.aa_kind_;
	aa_value_ = param.aa_value_;

	auto window_param = Ren3dUtilsCreateWindowParam{};
	window_param.renderer_kind_ = kind_;
	window_param.window_ = param.window_;
	window_param.aa_kind_ = aa_kind_;
	window_param.aa_value_ = aa_value_;

	if (window_param.window_.rect_2d_.extent_.width_ == 0 ||
		window_param.window_.rect_2d_.extent_.height_ == 0)
	{
		window_param.window_.rect_2d_.extent_.width_ = 1;
		window_param.window_.rect_2d_.extent_.height_ = 1;
	}

	if (window_param.aa_kind_ == Ren3dAaKind::ms)
	{
		if (device_features_.msaa_is_available_)
		{
			if (aa_value_ <= 0)
			{
				aa_value_ = device_features_.msaa_max_degree_;
			}

			if (aa_value_ < Ren3dLimits::aa_min_off)
			{
				aa_value_ = Ren3dLimits::aa_min_off;
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
				window_param.aa_kind_ = Ren3dAaKind::none;
				window_param.aa_value_ = 0;
				window_param.is_default_depth_buffer_disabled_ = true;
			}
		}
		else
		{
			window_param.aa_kind_ = Ren3dAaKind::none;
			window_param.aa_value_ = 0;
		}
	}

	Ren3dGlUtils::create_window_and_context(window_param, sdl_window_, sdl_gl_context_);

	Ren3dGlUtils::window_get_drawable_size(
		sdl_window_.get(),
		screen_width_,
		screen_height_);

	if (screen_width_ == 0 || screen_height_ == 0)
	{
		throw Exception{"Failed to get screen size."};
	}

	if (aa_kind_ == Ren3dAaKind::ms && device_features_.msaa_is_render_to_window_)
	{
		aa_value_ = Ren3dGlUtils::msaa_window_get_value();
	}

	extension_manager_ = Ren3dGlExtensionMgrFactory::create();

	if (extension_manager_ == nullptr)
	{
		throw Exception{"Failed to create an extension manager."};
	}

	switch (kind_)
	{
		case Ren3dKind::gl_2_0:
			extension_manager_->probe(Ren3dGlExtensionId::v2_0);

			if (!extension_manager_->has(Ren3dGlExtensionId::v2_0))
			{
				throw Exception{"Failed to load OpenGL 2.0 symbols."};
			}

			break;

		case Ren3dKind::gl_3_2_core:
			extension_manager_->probe(Ren3dGlExtensionId::v3_2_core);

			if (!extension_manager_->has(Ren3dGlExtensionId::v3_2_core))
			{
				throw Exception{"Failed to load OpenGL 3.2 core symbols."};
			}

			break;

		case Ren3dKind::gles_2_0:
			extension_manager_->probe(Ren3dGlExtensionId::es_v2_0);

			if (!extension_manager_->has(Ren3dGlExtensionId::es_v2_0))
			{
				throw Exception{"Failed to load OpenGL ES 2.0 symbols."};
			}

			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	Ren3dGlUtils::renderer_features_set(device_features_);

	gl_device_features_.context_kind_ = Ren3dGlUtils::context_get_kind();

	Ren3dGlUtils::anisotropy_probe(
		extension_manager_.get(),
		device_features_
	);

	Ren3dGlUtils::npot_probe(
		extension_manager_.get(),
		device_features_
	);

	Ren3dGlUtils::mipmap_probe(
		extension_manager_.get(),
		device_features_,
		gl_device_features_
	);

	Ren3dGlUtils::framebuffer_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	Ren3dGlUtils::sampler_probe(
		extension_manager_.get(),
		device_features_
	);

	Ren3dGlUtils::vertex_input_probe_max_locations(device_features_);

	Ren3dGlUtils::buffer_storage_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	Ren3dGlUtils::dsa_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	Ren3dGlUtils::sso_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	if (device_features_.vertex_input_max_locations_ <= 0)
	{
		throw Exception{"No vertex input locations."};
	}

	Ren3dGlUtils::vsync_probe(device_features_);

	Ren3dGlUtils::vertex_input_vao_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	gl_context_ = Ren3dGlContextFactory::create(
		kind_,
		device_features_,
		gl_device_features_
	);

	if (device_features_.vsync_is_available_)
	{
		static_cast<void>(Ren3dGlUtils::enable_vsync(param.is_vsync_));
	}

	framebuffers_create();

	device_info_ = Ren3dGlUtils::device_info_get();

	set_name_and_description();


	// Present.
	//
	gl_context_->clear(Rgba8{});
	present();
}

Ren3dGl::~Ren3dGl() = default;

Ren3dKind Ren3dGl::get_kind() const noexcept
{
	return kind_;
}

const std::string& Ren3dGl::get_name() const noexcept
{
	return name_;
}

const std::string& Ren3dGl::get_description() const noexcept
{
	return description_;
}

void Ren3dGl::fbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteFramebuffers ? glDeleteFramebuffers : glDeleteFramebuffersEXT);
	gl_function(1, &gl_name);
	Ren3dGlError::ensure_debug();
}

void Ren3dGl::rbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteRenderbuffers ? glDeleteRenderbuffers : glDeleteRenderbuffersEXT);
	gl_function(1, &gl_name);
	Ren3dGlError::ensure_debug();
}

void Ren3dGl::set_name_and_description()
{
	switch (kind_)
	{
		case Ren3dKind::gl_2_0:
			name_ = "GL2";
			description_ = "OpenGL 2.0+";
			break;

		case Ren3dKind::gl_3_2_core:
			name_ = "GL3.2C";
			description_ = "OpenGL 3.2 core";
			break;

		case Ren3dKind::gles_2_0:
			name_ = "GLES2.0";
			description_ = "OpenGL ES 2.0";
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

const Ren3dDeviceFeatures& Ren3dGl::get_device_features() const noexcept
{
	return device_features_;
}

const Ren3dDeviceInfo& Ren3dGl::get_device_info() const noexcept
{
	return device_info_;
}

void Ren3dGl::set_window_mode(
	const Ren3dSetWindowModeParam& param)
{
	Ren3dUtils::set_window_mode(sdl_window_.get(), param);

	const auto size_changed = (
		screen_width_ != param.rect_2d_.extent_.width_ ||
		screen_height_ != param.rect_2d_.extent_.height_);

	screen_width_ = param.rect_2d_.extent_.width_;
	screen_height_ = param.rect_2d_.extent_.height_;

	if (size_changed && gl_device_features_.framebuffer_is_available_)
	{
		msaa_framebuffer_destroy();
		msaa_framebuffer_create();
	}
}

void Ren3dGl::set_window_title(
	const std::string& title_utf8)
{
	Ren3dUtils::set_window_title(sdl_window_.get(), title_utf8);
}

void Ren3dGl::show_window(
	const bool is_visible)
{
	Ren3dUtils::show_window(sdl_window_.get(), is_visible);
}

bool Ren3dGl::get_vsync() const noexcept
{
	if (!device_features_.vsync_is_available_)
	{
		return false;
	}

	return Ren3dGlUtils::get_vsync();
}

void Ren3dGl::enable_vsync(
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

	if (!Ren3dGlUtils::enable_vsync(is_enabled))
	{
		throw Exception{"Not supported."};
	}
}

void Ren3dGl::set_anti_aliasing(
	const Ren3dAaKind aa_kind,
	const int aa_value)
{
	switch (aa_kind)
	{
		case Ren3dAaKind::none:
		case Ren3dAaKind::ms:
			break;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}

	auto clamped_aa_value = aa_value;

	if (clamped_aa_value < Ren3dLimits::aa_min_off)
	{
		clamped_aa_value = Ren3dLimits::aa_min_off;
	}

	if (clamped_aa_value > Ren3dLimits::aa_max)
	{
		clamped_aa_value = Ren3dLimits::aa_max;
	}

	switch (aa_kind)
	{
		case Ren3dAaKind::none:
			aa_disable();
			return;

		case Ren3dAaKind::ms:
			msaa_set(clamped_aa_value);
			return;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}
}

void Ren3dGl::present()
{
	framebuffers_blit();

	Ren3dGlError::ensure();

	Ren3dGlUtils::swap_window(sdl_window_.get());

	framebuffers_bind();
}

Ren3dBufferUPtr Ren3dGl::create_buffer(
	const Ren3dCreateBufferParam& param)
{
	return gl_context_->buffer_get_manager()->create_buffer(param);
}

Ren3dVertexInputUPtr Ren3dGl::create_vertex_input(
	const Ren3dCreateVertexInputParam& param)
{
	return gl_context_->vertex_input_get_manager()->create(param);
}

Ren3dShaderUPtr Ren3dGl::create_shader(
	const Ren3dCreateShaderParam& param)
{
	return gl_context_->shader_get_manager()->create(param);
}

Ren3dShaderStageUPtr Ren3dGl::create_shader_stage(
	const Ren3dCreateShaderStageParam& param)
{
	return gl_context_->shader_stage_get_manager()->create(param);
}

Ren3dTexture2dUPtr Ren3dGl::create_texture_2d(
	const Ren3dCreateTexture2dParam& param)
{
	return gl_context_->texture_get_manager()->create(param);
}

Ren3dSamplerUPtr Ren3dGl::create_sampler(
	const Ren3dCreateSamplerParam& param)
{
	return gl_context_->sampler_get_manager()->create(param);
}

Ren3dGl::RboResource Ren3dGl::renderbuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_function = (gl_device_features_.framebuffer_is_ext_ ? glGenRenderbuffersEXT : glGenRenderbuffers);

	auto gl_name = GLuint{};
	gl_function(1, &gl_name);
	Ren3dGlError::ensure_debug();

	auto rbo_resource = RboResource{gl_name};

	if (!rbo_resource)
	{
		throw Exception{"Failed to create OpenGL renderbuffer object."};
	}

	return rbo_resource;
}

void Ren3dGl::renderbuffer_bind(
	const GLuint gl_renderbuffer_name)
{
	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindRenderbufferEXT : glBindRenderbuffer);

	gl_func(GL_RENDERBUFFER, gl_renderbuffer_name);
	Ren3dGlError::ensure_debug();
}

Ren3dGl::FboResource Ren3dGl::framebuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glGenFramebuffersEXT : glGenFramebuffers);

	auto gl_name = GLuint{};
	gl_func(1, &gl_name);
	Ren3dGlError::ensure_debug();

	auto fbo_resource = FboResource{gl_name};

	if (!fbo_resource)
	{
		throw Exception{"Failed to create OpenGL framebuffer object."};
	}

	return fbo_resource;
}

void Ren3dGl::framebuffer_bind(
	const GLenum gl_target,
	const GLuint gl_name)
{
	assert(gl_device_features_.framebuffer_is_available_);

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindFramebufferEXT : glBindFramebuffer);

	gl_func(gl_target, gl_name);
	Ren3dGlError::ensure_debug();
}

void Ren3dGl::framebuffer_blit(
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

	Ren3dGlError::ensure_debug();
}

Ren3dGl::RboResource Ren3dGl::renderbuffer_create(
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
	Ren3dGlError::ensure_debug();

	renderbuffer_bind(0);

	return rbo_resource;
}

void Ren3dGl::msaa_color_rb_destroy()
{
	gl_msaa_color_rb_.reset();
}

void Ren3dGl::msaa_depth_rb_destroy()
{
	gl_msaa_depth_rb_.reset();
}

void Ren3dGl::msaa_fbo_destroy()
{
	gl_msaa_fbo_.reset();
}

void Ren3dGl::msaa_framebuffer_destroy()
{
	msaa_fbo_destroy();
	msaa_color_rb_destroy();
	msaa_depth_rb_destroy();
}

void Ren3dGl::msaa_color_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_color_rb_ = renderbuffer_create(width, height, sample_count, GL_RGBA8);
}

void Ren3dGl::msaa_depth_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_depth_rb_ = renderbuffer_create(width, height, sample_count, GL_DEPTH_COMPONENT);
}

void Ren3dGl::msaa_framebuffer_create()
{
	auto aa_degree = aa_value_;

	if (aa_kind_ == Ren3dAaKind::none)
	{
		aa_degree = Ren3dLimits::aa_min_off;
	}

	if (aa_degree < Ren3dLimits::aa_min_on)
	{
		aa_degree = Ren3dLimits::aa_min_off;
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

void Ren3dGl::framebuffers_destroy()
{
	msaa_framebuffer_destroy();
}

void Ren3dGl::framebuffers_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		return;
	}

	msaa_framebuffer_create();
}

void Ren3dGl::framebuffers_blit()
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

void Ren3dGl::framebuffers_bind()
{
	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	framebuffer_bind(GL_FRAMEBUFFER, gl_msaa_fbo_);
}

void Ren3dGl::aa_disable()
{
	aa_kind_ = Ren3dAaKind::none;

	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	msaa_framebuffer_destroy();
	msaa_framebuffer_create();
}

void Ren3dGl::msaa_set(
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

	if (aa_kind_ == Ren3dAaKind::ms && aa_value_ == aa_value)
	{
		return;
	}

	aa_kind_ = Ren3dAaKind::ms;
	aa_value_ = aa_value;

	framebuffers_destroy();

	framebuffers_create();
}

void Ren3dGl::command_execute_clear(
	const Ren3dClearCmd& command)
{
	gl_context_->clear(command.clear_.color_);
}

void Ren3dGl::command_execute_culling(
	const Ren3dEnableCullingCmd& command)
{
	gl_context_->culling_enable(command.is_enable_);
}

void Ren3dGl::command_execute_depth_test(
	const Ren3dEnableDepthTestCmd& command)
{
	gl_context_->depth_test_enable(command.is_enable_);
}

void Ren3dGl::command_execute_depth_write(
	const Ren3dEnableDepthWriteCmd& command)
{
	gl_context_->depth_write_enable(command.is_enable_);
}

void Ren3dGl::command_execute_viewport(
	const Ren3dSetViewportCmd& command)
{
	gl_context_->viewport_set(command.viewport_);
}

void Ren3dGl::command_execute_blending(
	const Ren3dEnableBlendingCmd& command)
{
	gl_context_->blending_enable(command.is_enable_);
}

void Ren3dGl::command_execute_blending_func(
	const Ren3dSetBlendingFuncCmd& command)
{
	gl_context_->blending_set_func(command.blending_func_);
}

void Ren3dGl::command_execute_scissor(
	const Ren3dEnableScissorCmd& command)
{
	gl_context_->scissor_enable(command.is_enable_);
}

void Ren3dGl::command_execute_scissor_box(
	const Ren3dSetScissorBoxCmd& command)
{
	gl_context_->scissor_set_box(command.scissor_box_);
}

void Ren3dGl::command_execute_texture(
	const Ren3dSetTextureCmd& command)
{
	gl_context_->texture_get_manager()->set_current(command.texture_2d_);
	gl_context_->texture_get_manager()->set(command.texture_2d_);
}

void Ren3dGl::command_execute_sampler(
	const Ren3dSetSamplerCmd& command)
{
	gl_context_->sampler_get_manager()->set(command.sampler_);
}

void Ren3dGl::command_execute_vertex_input(
	const Ren3dSetVertexInputCmd& command)
{
	gl_context_->vertex_input_get_manager()->set(command.vertex_input_);
}

void Ren3dGl::command_execute_shader_stage(
	const Ren3dSetShaderStageCmd& command)
{
	gl_context_->shader_stage_get_manager()->set_current(command.shader_stage_);
	gl_context_->shader_stage_get_manager()->set(command.shader_stage_);
}

void Ren3dGl::command_execute_shader_var_int32(
	const Ren3dSetInt32UniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_int(command.value_);
}

void Ren3dGl::command_execute_shader_var_float32(
	const Ren3dSetFloat32UniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_float(command.value_);
}

void Ren3dGl::command_execute_shader_var_vec2(
	const Ren3dSetVec2UniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec2(command.value_.data());
}

void Ren3dGl::command_execute_shader_var_vec4(
	const Ren3dSetVec4UniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec4(command.value_.data());
}

void Ren3dGl::command_execute_shader_var_mat4(
	const Ren3dSetMat4UniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_mat4(command.value_.data());
}

void Ren3dGl::command_execute_shader_var_sampler_2d(
	const Ren3dSetSampler2dUniformCmd& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_sampler_2d(command.value_);
}

void Ren3dGl::command_execute_draw_indexed(
	const Ren3dDrawIndexedCmd& command)
{
	const auto& param = command.draw_indexed_;

	auto gl_primitive_topology = GLenum{};

	switch (param.primitive_topology_)
	{
		case Ren3dPrimitiveTopology::point_list:
			gl_primitive_topology = GL_POINTS;
			break;

		case Ren3dPrimitiveTopology::line_list:
			gl_primitive_topology = GL_LINES;
			break;

		case Ren3dPrimitiveTopology::line_strip:
			gl_primitive_topology = GL_LINE_STRIP;
			break;

		case Ren3dPrimitiveTopology::triangle_list:
			gl_primitive_topology = GL_TRIANGLES;
			break;

		case Ren3dPrimitiveTopology::triangle_strip:
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

	auto index_buffer = static_cast<Ren3dGlBufferPtr>(gl_context_->vertex_input_get_manager()->get_current_index_buffer());

	if (!index_buffer)
	{
		throw Exception{"Null index buffer."};
	}

	gl_context_->shader_stage_get_manager()->set_to_current();
	gl_context_->texture_get_manager()->set_to_current();

	const auto index_buffer_offset = param.index_buffer_offset_ + (param.index_offset_ * param.index_byte_depth_);
	const auto index_buffer_indices = reinterpret_cast<const void*>(static_cast<std::intptr_t>(index_buffer_offset));

	const auto gl_element_type = Ren3dGlUtils::index_buffer_get_element_type_by_byte_depth(
		param.index_byte_depth_);

	index_buffer->set(true);

	glDrawElements(
		gl_primitive_topology, // mode
		param.vertex_count_, // count
		gl_element_type, // type
		index_buffer_indices // indices
	);

	Ren3dGlError::ensure_debug();
}

void Ren3dGl::submit_commands(
	const Ren3dCmdQueuePtr command_queue)
{
	const auto buffer_count = command_queue->get_count();

	for (int i = 0; i < buffer_count; ++i)
	{
		auto command_buffer = command_queue->get(i);

		if (!command_buffer->is_enabled())
		{
			continue;
		}

		const auto command_count = command_buffer->get_count();

		command_buffer->begin_read();

		for (int j = 0; j < command_count; ++j)
		{
			const auto command_id = command_buffer->read_command_id();

			switch (command_id)
			{
			case Ren3dCmdId::clear:
				command_execute_clear(*command_buffer->read_clear());
				break;

			case Ren3dCmdId::culling:
				command_execute_culling(*command_buffer->read_enable_culling());
				break;

			case Ren3dCmdId::depth_set_test:
				command_execute_depth_test(*command_buffer->read_enable_depth_test());
				break;

			case Ren3dCmdId::depth_set_write:
				command_execute_depth_write(*command_buffer->read_enable_depth_write());
				break;

			case Ren3dCmdId::viewport:
				command_execute_viewport(*command_buffer->read_set_viewport());
				break;

			case Ren3dCmdId::scissor:
				command_execute_scissor(*command_buffer->read_enable_scissor());
				break;

			case Ren3dCmdId::scissor_set_box:
				command_execute_scissor_box(*command_buffer->read_set_scissor_box());
				break;

			case Ren3dCmdId::blending:
				command_execute_blending(*command_buffer->read_enable_blending());
				break;

			case Ren3dCmdId::blending_func:
				command_execute_blending_func(*command_buffer->read_set_blending_func());
				break;

			case Ren3dCmdId::texture:
				command_execute_texture(*command_buffer->read_set_texture());
				break;

			case Ren3dCmdId::sampler:
				command_execute_sampler(*command_buffer->read_set_sampler());
				break;

			case Ren3dCmdId::vertex_input:
				command_execute_vertex_input(*command_buffer->read_set_vertex_input());
				break;

			case Ren3dCmdId::shader_stage:
				command_execute_shader_stage(*command_buffer->read_set_shader_stage());
				break;

			case Ren3dCmdId::shader_var_int32:
				command_execute_shader_var_int32(*command_buffer->read_set_int32_uniform());
				break;

			case Ren3dCmdId::shader_var_float32:
				command_execute_shader_var_float32(*command_buffer->read_set_float32_uniform());
				break;

			case Ren3dCmdId::shader_var_vec2:
				command_execute_shader_var_vec2(*command_buffer->read_set_vec2_uniform());
				break;

			case Ren3dCmdId::shader_var_vec4:
				command_execute_shader_var_vec4(*command_buffer->read_set_vec4_uniform());
				break;

			case Ren3dCmdId::shader_var_mat4:
				command_execute_shader_var_mat4(*command_buffer->read_set_mat4_uniform());
				break;

			case Ren3dCmdId::shader_var_sampler_2d:
				command_execute_shader_var_sampler_2d(*command_buffer->read_set_sampler_2d_uniform());
				break;

			case Ren3dCmdId::draw_indexed:
				command_execute_draw_indexed(*command_buffer->read_draw_indexed());
				break;

			default:
				throw Exception{"Unsupported command id."};
			}
		}

		command_buffer->end_read();
	}
}

//
// Ren3dGl
// ==========================================================================


} // detail
} // bstone
