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
#include "bstone_detail_gl_renderer_3d.h"

#include <cassert>

#include "bstone_exception.h"
#include "bstone_renderer_3d_limits.h"
#include "bstone_renderer_3d_tests.h"

#include "bstone_detail_gl_buffer_manager.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_sampler_manager.h"
#include "bstone_detail_gl_texture_manager.h"
#include "bstone_detail_gl_vertex_input_manager.h"
#include "bstone_detail_gl_shader_manager.h"
#include "bstone_detail_gl_shader_stage_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlRenderer3d
//

GlRenderer3d::GlRenderer3d(
	const Renderer3dCreateParam& param)
	:
	kind_{},
	name_{},
	description_{},
	sdl_window_{},
	sdl_gl_context_{},
	extension_manager_{},
	gl_context_{},
	device_info_{},
	device_features_{},
	gl_device_features_{},
	screen_width_{},
	screen_height_{},
	aa_kind_{},
	aa_value_{},
	gl_msaa_fbo_{},
	gl_msaa_color_rb_{},
	gl_msaa_depth_rb_{}
{
	initialize(param);
}

GlRenderer3d::~GlRenderer3d()
{
	uninitialize_internal();
}

Renderer3dKind GlRenderer3d::get_kind() const noexcept
{
	return kind_;
}

const std::string& GlRenderer3d::get_name() const noexcept
{
	return name_;
}

const std::string& GlRenderer3d::get_description() const noexcept
{
	return description_;
}

void GlRenderer3d::fbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteFramebuffers ? glDeleteFramebuffers : glDeleteFramebuffersEXT);
	gl_function(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlRenderer3d::rbo_resource_deleter(
	const GLuint& gl_name) noexcept
{
	const auto gl_function = (glDeleteRenderbuffers ? glDeleteRenderbuffers : glDeleteRenderbuffersEXT);
	gl_function(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlRenderer3d::set_name_and_description()
{
	switch (kind_)
	{
		case Renderer3dKind::gl_2_0:
			name_ = "GL2";
			description_ = "OpenGL 2.0+";
			break;

		case Renderer3dKind::gl_3_2_core:
			name_ = "GL3.2C";
			description_ = "OpenGL 3.2 core";
			break;

		case Renderer3dKind::gles_2_0:
			name_ = "GLES2.0";
			description_ = "OpenGL ES 2.0";
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

void GlRenderer3d::initialize(
	const Renderer3dCreateParam& param)
{
	switch (param.renderer_kind_)
	{
		case Renderer3dKind::gl_2_0:
		case Renderer3dKind::gl_3_2_core:
		case Renderer3dKind::gles_2_0:
			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	kind_ = param.renderer_kind_;

	GlRenderer3dUtils::msaa_probe(kind_, device_features_, gl_device_features_);

	aa_kind_ = param.aa_kind_;
	aa_value_ = param.aa_value_;

	auto window_param = Renderer3dUtilsCreateWindowParam{};
	window_param.renderer_kind_ = kind_;
	window_param.window_ = param.window_;
	window_param.aa_kind_ = aa_kind_;
	window_param.aa_value_ = aa_value_;

	if (window_param.window_.width_ == 0 || window_param.window_.height_ == 0)
	{
		window_param.window_.width_ = 1;
		window_param.window_.height_ = 1;
	}

	if (window_param.aa_kind_ == Renderer3dAaKind::ms)
	{
		if (device_features_.msaa_is_available_)
		{
			if (aa_value_ <= 0)
			{
				aa_value_ = device_features_.msaa_max_degree_;
			}

			if (aa_value_ < Renderer3dLimits::aa_min_off)
			{
				aa_value_ = Renderer3dLimits::aa_min_off;
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
				window_param.aa_kind_ = Renderer3dAaKind::none;
				window_param.aa_value_ = 0;
				window_param.is_default_depth_buffer_disabled_ = true;
			}
		}
		else
		{
			window_param.aa_kind_ = Renderer3dAaKind::none;
			window_param.aa_value_ = 0;
		}
	}

	GlRenderer3dUtils::create_window_and_context(window_param, sdl_window_, sdl_gl_context_);

	GlRenderer3dUtils::window_get_drawable_size(
		sdl_window_.get(),
		screen_width_,
		screen_height_);

	if (screen_width_ == 0 || screen_height_ == 0)
	{
		throw Exception{"Failed to get screen size."};
	}

	if (aa_kind_ == Renderer3dAaKind::ms && device_features_.msaa_is_render_to_window_)
	{
		aa_value_ = GlRenderer3dUtils::msaa_window_get_value();
	}

	extension_manager_ = detail::GlExtensionManagerFactory::create();

	if (extension_manager_ == nullptr)
	{
		throw Exception{"Failed to create an extension manager."};
	}

	switch (kind_)
	{
		case Renderer3dKind::gl_2_0:
			extension_manager_->probe(GlExtensionId::v2_0);

			if (!extension_manager_->has(GlExtensionId::v2_0))
			{
				throw Exception{"Failed to load OpenGL 2.0 symbols."};
			}

			break;

		case Renderer3dKind::gl_3_2_core:
			extension_manager_->probe(GlExtensionId::v3_2_core);

			if (!extension_manager_->has(GlExtensionId::v3_2_core))
			{
				throw Exception{"Failed to load OpenGL 3.2 core symbols."};
			}

			break;

		case Renderer3dKind::gles_2_0:
			extension_manager_->probe(GlExtensionId::es_v2_0);

			if (!extension_manager_->has(GlExtensionId::es_v2_0))
			{
				throw Exception{"Failed to load OpenGL ES 2.0 symbols."};
			}

			break;

		default:
			throw Exception{"Unsupported renderer kind."};
	}

	GlRenderer3dUtils::renderer_features_set(device_features_);

	gl_device_features_.context_kind_ = GlRenderer3dUtils::context_get_kind();

	GlRenderer3dUtils::anisotropy_probe(
		extension_manager_.get(),
		device_features_
	);

	GlRenderer3dUtils::npot_probe(
		extension_manager_.get(),
		device_features_
	);

	GlRenderer3dUtils::mipmap_probe(
		extension_manager_.get(),
		device_features_,
		gl_device_features_
	);

	GlRenderer3dUtils::framebuffer_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	GlRenderer3dUtils::sampler_probe(
		extension_manager_.get(),
		device_features_
	);

	GlRenderer3dUtils::vertex_input_probe_max_locations(device_features_);

	GlRenderer3dUtils::buffer_storage_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	GlRenderer3dUtils::dsa_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	GlRenderer3dUtils::sso_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	if (device_features_.vertex_input_max_locations_ <= 0)
	{
		throw Exception{"No vertex input locations."};
	}

	GlRenderer3dUtils::vsync_probe(device_features_);

	GlRenderer3dUtils::vertex_input_vao_probe(
		extension_manager_.get(),
		gl_device_features_
	);

	gl_context_ = GlContextFactory::create(
		kind_,
		device_features_,
		gl_device_features_
	);

	if (device_features_.vsync_is_available_)
	{
		static_cast<void>(GlRenderer3dUtils::vsync_set(param.is_vsync_));
	}

	framebuffers_create();

	device_info_ = GlRenderer3dUtils::device_info_get();

	set_name_and_description();


	// Present.
	//
	gl_context_->clear(Rgba8{});
	present();
}

void GlRenderer3d::uninitialize()
{
	uninitialize_internal();
}

const Renderer3dDeviceFeatures& GlRenderer3d::device_get_features() const noexcept
{
	return device_features_;
}

const Renderer3dDeviceInfo& GlRenderer3d::device_get_info() const noexcept
{
	return device_info_;
}

void GlRenderer3d::window_set_mode(
	const Renderer3dWindowSetModeParam& param)
{
	Renderer3dUtils::window_set_mode(sdl_window_.get(), param);

	const auto size_changed = (screen_width_ != param.width_ || screen_height_ != param.height_);

	screen_width_ = param.width_;
	screen_height_ = param.height_;

	if (size_changed && gl_device_features_.framebuffer_is_available_)
	{
		msaa_framebuffer_destroy();
		msaa_framebuffer_create();
	}
}

void GlRenderer3d::window_set_title(
	const std::string& title_utf8)
{
	Renderer3dUtils::window_set_title(sdl_window_.get(), title_utf8);
}

void GlRenderer3d::window_show(
	const bool is_visible)
{
	Renderer3dUtils::window_show(sdl_window_.get(), is_visible);
}

bool GlRenderer3d::vsync_get() const noexcept
{
	if (!device_features_.vsync_is_available_)
	{
		return false;
	}

	return GlRenderer3dUtils::vsync_get();
}

void GlRenderer3d::vsync_set(
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

	if (!GlRenderer3dUtils::vsync_set(is_enabled))
	{
		throw Exception{"Not supported."};
	}
}

void GlRenderer3d::aa_set(
	const Renderer3dAaKind aa_kind,
	const int aa_value)
{
	switch (aa_kind)
	{
		case Renderer3dAaKind::none:
		case Renderer3dAaKind::ms:
			break;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}

	auto clamped_aa_value = aa_value;

	if (clamped_aa_value < Renderer3dLimits::aa_min_off)
	{
		clamped_aa_value = Renderer3dLimits::aa_min_off;
	}

	if (clamped_aa_value > Renderer3dLimits::aa_max)
	{
		clamped_aa_value = Renderer3dLimits::aa_max;
	}

	switch (aa_kind)
	{
		case Renderer3dAaKind::none:
			aa_disable();
			return;

		case Renderer3dAaKind::ms:
			msaa_set(clamped_aa_value);
			return;

		default:
			throw Exception{"Invalid anti-aliasing kind."};
	}
}

void GlRenderer3d::present()
{
	framebuffers_blit();

	GlRenderer3dUtils::swap_window(sdl_window_.get());

	framebuffers_bind();
}

Renderer3dBufferUPtr GlRenderer3d::buffer_create(
	const Renderer3dBufferCreateParam& param)
{
	return gl_context_->buffer_get_manager()->buffer_create(param);
}

Renderer3dVertexInputUPtr GlRenderer3d::vertex_input_create(
	const Renderer3dVertexInputCreateParam& param)
{
	return gl_context_->vertex_input_get_manager()->create(param);
}

Renderer3dShaderUPtr GlRenderer3d::shader_create(
	const Renderer3dShaderCreateParam& param)
{
	return gl_context_->shader_get_manager()->create(param);
}

Renderer3dShaderStageUPtr GlRenderer3d::shader_stage_create(
	const Renderer3dShaderStageCreateParam& param)
{
	return gl_context_->shader_stage_get_manager()->create(param);
}

void GlRenderer3d::clear(
	const Renderer3dClearParam& param)
{
	gl_context_->clear(param.color_);
}

void GlRenderer3d::viewport_set(
	const Renderer3dViewport& viewport)
{
	gl_context_->viewport_set(viewport);
}

void GlRenderer3d::scissor_enable(
	const bool is_enable)
{
	gl_context_->scissor_enable(is_enable);
}

void GlRenderer3d::scissor_box_set(
	const Renderer3dScissorBox& scissor_box)
{
	gl_context_->scissor_set_box(scissor_box);
}

void GlRenderer3d::culling_enable(
	const bool is_enable)
{
	gl_context_->culling_enable(is_enable);
}

void GlRenderer3d::depth_test_enable(
	const bool is_enable)
{
	gl_context_->depth_test_enable(is_enable);
}

void GlRenderer3d::depth_write_enable(
	const bool is_enable)
{
	gl_context_->depth_write_enable(is_enable);
}

void GlRenderer3d::blending_enable(
	const bool is_enable)
{
	gl_context_->blending_enable(is_enable);
}

void GlRenderer3d::blending_function_set(
	const Renderer3dBlendingFunc& blending_function)
{
	gl_context_->blending_set_func(blending_function);
}

void GlRenderer3d::texture_2d_set(
	const Renderer3dTexture2dPtr texture_2d)
{
	gl_context_->texture_get_manager()->set(texture_2d);
}

void GlRenderer3d::sampler_set(
	const Renderer3dSamplerPtr sampler)
{
	gl_context_->sampler_get_manager()->set(sampler);
}

void GlRenderer3d::vertex_input_set(
	const Renderer3dVertexInputPtr vertex_input)
{
	gl_context_->vertex_input_get_manager()->set(vertex_input);
}

void GlRenderer3d::shader_stage_set(
	const Renderer3dShaderStagePtr shader_stage)
{
	gl_context_->shader_stage_get_manager()->set_current(shader_stage);
}

void GlRenderer3d::draw_indexed(
	const Renderer3dDrawIndexedParam& param)
{
	auto gl_primitive_topology = GLenum{};

	switch (param.primitive_topology_)
	{
		case Renderer3dPrimitiveTopology::point_list:
			gl_primitive_topology = GL_POINTS;
			break;

		case Renderer3dPrimitiveTopology::line_list:
			gl_primitive_topology = GL_LINES;
			break;

		case Renderer3dPrimitiveTopology::line_strip:
			gl_primitive_topology = GL_LINE_STRIP;
			break;

		case Renderer3dPrimitiveTopology::triangle_list:
			gl_primitive_topology = GL_TRIANGLES;
			break;

		case Renderer3dPrimitiveTopology::triangle_strip:
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

	auto index_buffer = static_cast<GlBufferPtr>(gl_context_->vertex_input_get_manager()->get_current_index_buffer());

	if (!index_buffer)
	{
		throw Exception{"Null index buffer."};
	}

	const auto shader_stage = gl_context_->shader_stage_get_manager()->get_current();

	if (!shader_stage)
	{
		throw Exception{"Null current shader stage."};
	}

	shader_stage->set();

	const auto index_buffer_offset = param.index_buffer_offset_ + (param.index_offset_ * param.index_byte_depth_);
	const auto index_buffer_indices = reinterpret_cast<const void*>(static_cast<std::intptr_t>(index_buffer_offset));

	const auto gl_element_type = GlRenderer3dUtils::index_buffer_get_element_type_by_byte_depth(
		param.index_byte_depth_);

	index_buffer->set(true);

	glDrawElements(
		gl_primitive_topology, // mode
		param.vertex_count_, // count
		gl_element_type, // type
		index_buffer_indices // indices
	);

	assert(!GlRenderer3dUtils::was_errors());
}

Renderer3dTexture2dUPtr GlRenderer3d::texture_2d_create(
	const Renderer3dTexture2dCreateParam& param)
{
	return gl_context_->texture_get_manager()->create(param);
}

Renderer3dSamplerUPtr GlRenderer3d::sampler_create(
	const Renderer3dSamplerCreateParam& param)
{
	return gl_context_->sampler_get_manager()->create(param);
}

void GlRenderer3d::uninitialize_internal()
{
	kind_ = {};
	gl_context_ = {};
	extension_manager_ = {};

	framebuffers_destroy();

	if (sdl_gl_context_)
	{
		GlRenderer3dUtils::make_context_current(sdl_window_.get(), nullptr);
	}

	sdl_gl_context_ = {};
	sdl_window_ = {};

	device_info_ = {};
	device_features_ = {};
	gl_device_features_ = {};
	screen_width_ = {};
	screen_height_ = {};
}

GlRenderer3d::RboResource GlRenderer3d::renderbuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_function = (gl_device_features_.framebuffer_is_ext_ ? glGenRenderbuffersEXT : glGenRenderbuffers);

	auto gl_name = GLuint{};
	gl_function(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());

	if (gl_name == 0)
	{
		throw Exception{"Failed to create OpenGL renderbuffer object."};
	}

	return RboResource{gl_name};
}

void GlRenderer3d::renderbuffer_bind(
	const GLuint gl_renderbuffer_name)
{
	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindRenderbufferEXT : glBindRenderbuffer);

	gl_func(GL_RENDERBUFFER, gl_renderbuffer_name);
	assert(!GlRenderer3dUtils::was_errors());
}

GlRenderer3d::FboResource GlRenderer3d::framebuffer_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		throw Exception{"Framebuffer not available."};
	}

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glGenFramebuffersEXT : glGenFramebuffers);

	auto gl_name = GLuint{};
	gl_func(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());

	if (gl_name == 0)
	{
		throw Exception{"Failed to create OpenGL framebuffer object."};
	}

	return FboResource{gl_name};
}

void GlRenderer3d::framebuffer_bind(
	const GLenum gl_target,
	const GLuint gl_name)
{
	assert(gl_device_features_.framebuffer_is_available_);

	const auto gl_func = (gl_device_features_.framebuffer_is_ext_ ? glBindFramebufferEXT : glBindFramebuffer);

	gl_func(gl_target, gl_name);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlRenderer3d::framebuffer_blit(
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

	assert(!GlRenderer3dUtils::was_errors());
}

GlRenderer3d::RboResource GlRenderer3d::renderbuffer_create(
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
	assert(!GlRenderer3dUtils::was_errors());

	renderbuffer_bind(0);

	return rbo_resource;
}

void GlRenderer3d::msaa_color_rb_destroy()
{
	gl_msaa_color_rb_.reset();
}

void GlRenderer3d::msaa_depth_rb_destroy()
{
	gl_msaa_depth_rb_.reset();
}

void GlRenderer3d::msaa_fbo_destroy()
{
	gl_msaa_fbo_.reset();
}

void GlRenderer3d::msaa_framebuffer_destroy()
{
	msaa_fbo_destroy();
	msaa_color_rb_destroy();
	msaa_depth_rb_destroy();
}

void GlRenderer3d::msaa_color_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_color_rb_ = renderbuffer_create(width, height, sample_count, GL_RGBA8);
}

void GlRenderer3d::msaa_depth_rb_create(
	const int width,
	const int height,
	const int sample_count)
{
	gl_msaa_depth_rb_ = renderbuffer_create(width, height, sample_count, GL_DEPTH_COMPONENT);
}

void GlRenderer3d::msaa_framebuffer_create()
{
	auto aa_degree = aa_value_;

	if (aa_kind_ == Renderer3dAaKind::none)
	{
		aa_degree = Renderer3dLimits::aa_min_off;
	}

	if (aa_degree < Renderer3dLimits::aa_min_on)
	{
		aa_degree = Renderer3dLimits::aa_min_off;
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

void GlRenderer3d::framebuffers_destroy()
{
	msaa_framebuffer_destroy();
}

void GlRenderer3d::framebuffers_create()
{
	if (!gl_device_features_.framebuffer_is_available_)
	{
		return;
	}

	msaa_framebuffer_create();
}

void GlRenderer3d::framebuffers_blit()
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

void GlRenderer3d::framebuffers_bind()
{
	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	framebuffer_bind(GL_FRAMEBUFFER, gl_msaa_fbo_);
}

void GlRenderer3d::aa_disable()
{
	aa_kind_ = Renderer3dAaKind::none;

	if (gl_msaa_fbo_ == 0)
	{
		return;
	}

	msaa_framebuffer_destroy();
	msaa_framebuffer_create();
}

void GlRenderer3d::msaa_set(
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

	if (aa_kind_ == Renderer3dAaKind::ms && aa_value_ == aa_value)
	{
		return;
	}

	aa_kind_ = Renderer3dAaKind::ms;
	aa_value_ = aa_value;

	framebuffers_destroy();

	framebuffers_create();
}

//
// GlRenderer3d
// ==========================================================================


} // detail
} // bstone
