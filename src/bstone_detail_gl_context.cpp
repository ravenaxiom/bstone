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
// OpenGL context (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_context.h"

#include <list>
#include <vector>

#include "bstone_exception.h"

#include "bstone_detail_gl_buffer.h"
#include "bstone_detail_gl_buffer_manager.h"
#include "bstone_detail_gl_device_features.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_sampler.h"
#include "bstone_detail_gl_sampler_manager.h"
#include "bstone_detail_gl_shader_manager.h"
#include "bstone_detail_gl_shader_stage_manager.h"
#include "bstone_detail_gl_texture_2d.h"
#include "bstone_detail_gl_texture_manager.h"
#include "bstone_detail_gl_vao_manager.h"
#include "bstone_detail_gl_vertex_input.h"
#include "bstone_detail_gl_vertex_input_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlContext
//

GlContext::GlContext() = default;

GlContext::~GlContext() = default;

//
// GlContext
// ==========================================================================


// ==========================================================================
// GlContextImpl
//

class GlContextImpl final :
	public GlContext
{
public:
	GlContextImpl(
		const Renderer3dDeviceFeatures& device_features,
		const GlDeviceFeatures& gl_device_features);

	GlContextImpl(
		const GlContextImpl& rhs) = delete;

	~GlContextImpl() override;


	const Renderer3dDeviceFeatures& get_device_features() const noexcept override;

	const GlDeviceFeatures& get_gl_device_features() const noexcept override;


	GlBufferManagerPtr buffer_get_manager() const noexcept override;

	GlSamplerManagerPtr sampler_get_manager() const noexcept override;

	GlTextureManagerPtr texture_get_manager() const noexcept override;

	GlVaoManagerPtr vao_get_manager() const noexcept override;

	GlVertexInputManagerPtr vertex_input_get_manager() const noexcept override;

	GlShaderManagerPtr shader_get_manager() const noexcept override;

	GlShaderStageManagerPtr shader_stage_get_manager() const noexcept override;


	void clear(
		const Rgba8& rgba_8) override;

	void viewport_set(
		const Renderer3dViewport& viewport) override;


	void scissor_enable(
		const bool is_enable) override;

	void scissor_set_box(
		const Renderer3dScissorBox& scissor_box) override;


	void culling_enable(
		const bool is_enable) override;


	void depth_test_enable(
		const bool is_enable) override;

	void depth_write_enable(
		const bool is_enable) override;


	void blending_enable(
		const bool is_enable) override;

	void blending_set_func(
		const Renderer3dBlendingFunc& func) override;


private:
	const Renderer3dDeviceFeatures& device_features_;
	const GlDeviceFeatures& gl_device_features_;

	Rgba8 clear_color_;

	Renderer3dViewport viewport_;

	bool scissor_is_enabled_;
	Renderer3dScissorBox scissor_box_;

	bool culling_is_enabled_;
	Renderer3dCullingFace culling_face_;
	Renderer3dCullingMode culling_mode_;

	bool depth_is_test_enabled_;
	bool depth_is_write_enabled_;

	bool blending_is_enabled_;
	Renderer3dBlendingFunc blending_func_;

	GlVaoManagerUPtr vao_manager_;
	GlBufferManagerUPtr buffer_manager_;
	GlVertexInputManagerUPtr vertex_input_manager_;
	GlSamplerManagerUPtr sampler_manager_;
	GlTextureManagerUPtr texture_manager_;
	GlShaderManagerUPtr shader_manager_;
	GlShaderStageManagerUPtr shader_stage_manager_;


	void initialize();

	void initialize_vertex_arrays();

	void initialize_buffers();

	void initialize_vertex_inputs();

	void initialize_samplers();

	void initialize_textures();

	void initialize_shaders();

	void initialize_shader_stages();

	void mipmap_set_max_quality();


	void clear_set_color();

	void clear();

	void clear_set_defaults();


	void viewport_set_rect();

	void viewport_set_depth_range();

	void viewport_set_defaults();


	void scissor_enable();

	void scissor_set_box();

	void scissor_set_defaults();


	void culling_enable();

	void culling_set_face();

	void culling_set_mode();

	void culling_set_defaults();


	void depth_set_test();

	void depth_set_write();

	void depth_set_defaults();


	void blending_enable();

	void blending_set_func();

	void blending_set_defaults();
}; // GlContextImpl

using GlContextImplPtr = GlContextImpl*;
using GlContextImplUPtr = std::unique_ptr<GlContextImpl>;

//
// GlContextImpl
// ==========================================================================


// ==========================================================================
// GlContextImpl
//

GlContextImpl::GlContextImpl(
	const Renderer3dDeviceFeatures& device_features,
	const GlDeviceFeatures& gl_device_features)
	:
	device_features_{device_features},
	gl_device_features_{gl_device_features},
	clear_color_{},
	viewport_{},
	scissor_is_enabled_{},
	scissor_box_{},
	culling_is_enabled_{},
	culling_face_{},
	culling_mode_{},
	depth_is_test_enabled_{},
	depth_is_write_enabled_{},
	blending_is_enabled_{},
	blending_func_{},
	vao_manager_{},
	buffer_manager_{},
	vertex_input_manager_{},
	sampler_manager_{},
	texture_manager_{}
{
	initialize();
}

GlContextImpl::~GlContextImpl() = default;

void GlContextImpl::initialize_vertex_arrays()
{
	vao_manager_ = GlVaoManagerFactory::create(this, device_features_, gl_device_features_);
}

void GlContextImpl::initialize_buffers()
{
	buffer_manager_ = GlBufferManagerFactory::create(this, vao_manager_.get());
}

void GlContextImpl::initialize_vertex_inputs()
{
	vertex_input_manager_ = GlVertexInputManagerFactory::create(this);
}

void GlContextImpl::initialize_samplers()
{
	sampler_manager_ = GlSamplerManagerFactory::create(this);
}

void GlContextImpl::initialize_textures()
{
	texture_manager_ = GlTextureManagerFactory::create(this);
}

void GlContextImpl::initialize_shaders()
{
	shader_manager_ = GlShaderManagerFactory::create(this);
}

void GlContextImpl::initialize_shader_stages()
{
	shader_stage_manager_ = GlShaderStageManagerFactory::create(this);
}

void GlContextImpl::mipmap_set_max_quality()
{
	if (!device_features_.mipmap_is_available_)
	{
		return;
	}

	if (gl_device_features_.context_kind_ == GlContextKind::core)
	{
		return;
	}

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlContextImpl::initialize()
{
	mipmap_set_max_quality();
	initialize_vertex_arrays();
	initialize_buffers();
	initialize_vertex_inputs();
	initialize_samplers();
	initialize_textures();
	initialize_shaders();
	initialize_shader_stages();

	clear_set_defaults();
	viewport_set_defaults();
	scissor_set_defaults();
	culling_set_defaults();
	depth_set_defaults();
	blending_set_defaults();
}

const Renderer3dDeviceFeatures& GlContextImpl::get_device_features() const noexcept
{
	return device_features_;
}

const GlDeviceFeatures& GlContextImpl::get_gl_device_features() const noexcept
{
	return gl_device_features_;
}

GlBufferManagerPtr GlContextImpl::buffer_get_manager() const noexcept
{
	return buffer_manager_.get();
}

GlSamplerManagerPtr GlContextImpl::sampler_get_manager() const noexcept
{
	return sampler_manager_.get();
}

GlTextureManagerPtr GlContextImpl::texture_get_manager() const noexcept
{
	return texture_manager_.get();
}

GlVaoManagerPtr GlContextImpl::vao_get_manager() const noexcept
{
	return vao_manager_.get();
}

GlVertexInputManagerPtr GlContextImpl::vertex_input_get_manager() const noexcept
{
	return vertex_input_manager_.get();
}

GlShaderManagerPtr GlContextImpl::shader_get_manager() const noexcept
{
	return shader_manager_.get();
}

GlShaderStageManagerPtr GlContextImpl::shader_stage_get_manager() const noexcept
{
	return shader_stage_manager_.get();
}

void GlContextImpl::clear(
	const Rgba8& rgba_8)
{
	if (clear_color_ != rgba_8)
	{
		clear_color_ = rgba_8;
		clear_set_color();
	}

	clear();
}

void GlContextImpl::viewport_set(
	const Renderer3dViewport& viewport)
{
	if (viewport_.x_ != viewport.x_ ||
		viewport_.y_ != viewport.y_ ||
		viewport_.width_ != viewport.width_ ||
		viewport_.height_ != viewport.height_)
	{
		viewport_.x_ = viewport.x_;
		viewport_.y_ = viewport.y_;
		viewport_.width_ = viewport.width_;
		viewport_.height_ = viewport.height_;
		viewport_set_rect();
	}

	if (viewport_.min_depth_ != viewport.min_depth_ ||
		viewport_.max_depth_ != viewport.max_depth_)
	{
		viewport_.min_depth_ = viewport.min_depth_;
		viewport_.max_depth_ = viewport.max_depth_;
		viewport_set_depth_range();
	}
}

void GlContextImpl::scissor_enable(
	const bool is_enable)
{
	if (scissor_is_enabled_ != is_enable)
	{
		scissor_is_enabled_ = is_enable;
		scissor_enable();
	}
}

void GlContextImpl::scissor_set_box(
	const Renderer3dScissorBox& scissor_box)
{
	if (scissor_box_.x_ != scissor_box.x_ ||
		scissor_box_.y_ != scissor_box.y_ ||
		scissor_box_.width_ != scissor_box.width_ ||
		scissor_box_.height_ != scissor_box.height_)
	{
		scissor_box_ = scissor_box;
		scissor_set_box();
	}
}

void GlContextImpl::culling_enable(
	const bool is_enable)
{
	if (culling_is_enabled_ != is_enable)
	{
		culling_is_enabled_ = is_enable;
		culling_enable();
	}
}

void GlContextImpl::depth_test_enable(
	const bool is_enable)
{
	if (depth_is_test_enabled_ != is_enable)
	{
		depth_is_test_enabled_ = is_enable;
		depth_set_test();
	}
}

void GlContextImpl::depth_write_enable(
	const bool is_enable)
{
	if (depth_is_write_enabled_ != is_enable)
	{
		depth_is_write_enabled_ = is_enable;
		depth_set_write();
	}
}


void GlContextImpl::blending_enable(
	const bool is_enable)
{
	if (blending_is_enabled_ != is_enable)
	{
		blending_is_enabled_ = is_enable;
		blending_enable();
	}
}

void GlContextImpl::blending_set_func(
	const Renderer3dBlendingFunc& blending_func)
{
	if (blending_func_.src_factor_ != blending_func.src_factor_ ||
		blending_func_.dst_factor_ != blending_func.dst_factor_)
	{
		blending_func_ = blending_func;
		blending_set_func();
	}
}

void GlContextImpl::clear_set_color()
{
	glClearColor(
		static_cast<float>(clear_color_.r_) / 255.0F,
		static_cast<float>(clear_color_.g_) / 255.0F,
		static_cast<float>(clear_color_.b_) / 255.0F,
		static_cast<float>(clear_color_.a_) / 255.0F
	);

	assert(!GlRenderer3dUtils::was_errors());
}

void GlContextImpl::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlContextImpl::clear_set_defaults()
{
	clear_color_ = {};
	clear_set_color();
}

void GlContextImpl::viewport_set_rect()
{
	GlRenderer3dUtils::viewport_set_rect(viewport_);
}

void GlContextImpl::viewport_set_depth_range()
{
	GlRenderer3dUtils::viewport_set_depth_range(viewport_, gl_device_features_);
}

void GlContextImpl::viewport_set_defaults()
{
	viewport_.x_ = 0;
	viewport_.y_ = 0;
	viewport_.width_ = 0;
	viewport_.height_ = 0;
	viewport_set_rect();

	viewport_.min_depth_ = 0.0F;
	viewport_.max_depth_ = 1.0F;
	viewport_set_depth_range();
}

void GlContextImpl::scissor_enable()
{
	GlRenderer3dUtils::scissor_enable(scissor_is_enabled_);
}

void GlContextImpl::scissor_set_box()
{
	GlRenderer3dUtils::scissor_set_box(scissor_box_);
}

void GlContextImpl::scissor_set_defaults()
{
	scissor_is_enabled_ = false;
	scissor_enable();

	scissor_box_.x_ = 0;
	scissor_box_.y_ = 0;
	scissor_box_.width_ = 0;
	scissor_box_.height_ = 0;
	scissor_set_box();
}

void GlContextImpl::culling_enable()
{
	GlRenderer3dUtils::culling_enable(culling_is_enabled_);
}

void GlContextImpl::culling_set_face()
{
	GlRenderer3dUtils::culling_set_face(culling_face_);
}

void GlContextImpl::culling_set_mode()
{
	GlRenderer3dUtils::culling_set_mode(culling_mode_);
}

void GlContextImpl::culling_set_defaults()
{
	culling_is_enabled_ = false;
	culling_enable();

	culling_face_ = Renderer3dCullingFace::counter_clockwise;
	culling_set_face();

	culling_mode_ = Renderer3dCullingMode::back;
	culling_set_mode();
}

void GlContextImpl::depth_set_test()
{
	GlRenderer3dUtils::depth_test_enable(depth_is_test_enabled_);
}

void GlContextImpl::depth_set_write()
{
	GlRenderer3dUtils::depth_write_enable(depth_is_write_enabled_);
}

void GlContextImpl::depth_set_defaults()
{
	depth_is_test_enabled_ = false;
	depth_set_test();

	depth_is_write_enabled_ = false;
	depth_set_write();
}

void GlContextImpl::blending_enable()
{
	GlRenderer3dUtils::blending_enable(blending_is_enabled_);
}

void GlContextImpl::blending_set_func()
{
	GlRenderer3dUtils::blending_set_func(blending_func_);
}

void GlContextImpl::blending_set_defaults()
{
	blending_is_enabled_ = false;
	blending_enable();

	blending_func_.src_factor_ = Renderer3dBlendingFactor::src_alpha;
	blending_func_.dst_factor_ = Renderer3dBlendingFactor::one_minus_src_alpha;
	blending_set_func();
}

//
// GlContextImpl
// ==========================================================================


// =========================================================================
// GlContextFactory
//

GlContextUPtr GlContextFactory::create(
	const Renderer3dKind renderer_kind,
	const Renderer3dDeviceFeatures& device_features,
	const GlDeviceFeatures& gl_device_features)
{
	switch (renderer_kind)
	{
		case Renderer3dKind::gl_2_0:
		case Renderer3dKind::gl_3_2_core:
		case Renderer3dKind::gles_2_0:
			return GlContextImplUPtr{new GlContextImpl{device_features, gl_device_features}};

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

//
// GlContextFactory
// =========================================================================


} // detail
} // bstone
