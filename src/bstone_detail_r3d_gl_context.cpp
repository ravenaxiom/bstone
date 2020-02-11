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
#include "bstone_detail_r3d_gl_context.h"

#include <list>
#include <vector>

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_buffer.h"
#include "bstone_detail_r3d_gl_buffer_mgr.h"
#include "bstone_detail_r3d_gl_device_features.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_sampler.h"
#include "bstone_detail_r3d_gl_sampler_mgr.h"
#include "bstone_detail_r3d_gl_shader_mgr.h"
#include "bstone_detail_r3d_gl_shader_stage_mgr.h"
#include "bstone_detail_r3d_gl_texture_2d.h"
#include "bstone_detail_r3d_gl_texture_mgr.h"
#include "bstone_detail_r3d_gl_vao_mgr.h"
#include "bstone_detail_r3d_gl_vertex_input.h"
#include "bstone_detail_r3d_gl_vertex_input_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlContextImpl
//

class GlContextImpl final :
	public Ren3dGlContext
{
public:
	GlContextImpl(
		const Ren3dDeviceFeatures& device_features,
		const Ren3dGlDeviceFeatures& gl_device_features);

	GlContextImpl(
		const GlContextImpl& rhs) = delete;

	~GlContextImpl() override;


	const Ren3dDeviceFeatures& get_device_features() const noexcept override;

	const Ren3dGlDeviceFeatures& get_gl_device_features() const noexcept override;


	Ren3dGlBufferMgrPtr buffer_get_manager() const noexcept override;

	Ren3dGlSamplerMgrPtr sampler_get_manager() const noexcept override;

	Ren3dGlTextureMgrPtr texture_get_manager() const noexcept override;

	Ren3dGlVaoMgrPtr vao_get_manager() const noexcept override;

	Ren3dGlVertexInputMgrPtr vertex_input_get_manager() const noexcept override;

	Ren3dGlShaderMgrPtr shader_get_manager() const noexcept override;

	Ren3dGlShaderStageMgrPtr shader_stage_get_manager() const noexcept override;


	void clear(
		const Rgba8& rgba_8) override;

	void viewport_set(
		const Ren3dViewport& viewport) override;


	void scissor_enable(
		const bool is_enable) override;

	void scissor_set_box(
		const Ren3dScissorBox& scissor_box) override;


	void culling_enable(
		const bool is_enable) override;


	void depth_test_enable(
		const bool is_enable) override;

	void depth_write_enable(
		const bool is_enable) override;


	void blending_enable(
		const bool is_enable) override;

	void blending_set_func(
		const Ren3dBlendingFunc& func) override;


private:
	const Ren3dDeviceFeatures& device_features_;
	const Ren3dGlDeviceFeatures& gl_device_features_;

	Rgba8 clear_color_;

	Ren3dViewport viewport_;

	bool scissor_is_enabled_;
	Ren3dScissorBox scissor_box_;

	bool culling_is_enabled_;
	Ren3dCullingFace culling_face_;
	Ren3dCullingMode culling_mode_;

	bool depth_is_test_enabled_;
	bool depth_is_write_enabled_;

	bool blending_is_enabled_;
	Ren3dBlendingFunc blending_func_;

	Ren3dGlVaoMgrUPtr vao_manager_;
	Ren3dGlBufferMgrUPtr buffer_manager_;
	Ren3dGlVertexInputMgrUPtr vertex_input_manager_;
	Ren3dGlSamplerMgrUPtr sampler_manager_;
	Ren3dGlTextureMgrUPtr texture_manager_;
	Ren3dGlShaderMgrUPtr shader_manager_;
	Ren3dGlShaderStageMgrUPtr shader_stage_manager_;


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
	const Ren3dDeviceFeatures& device_features,
	const Ren3dGlDeviceFeatures& gl_device_features)
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
	vao_manager_ = Ren3dGlVaoMgrFactory::create(this, device_features_, gl_device_features_);
}

void GlContextImpl::initialize_buffers()
{
	buffer_manager_ = Ren3dGlBufferMgrFactory::create(this, vao_manager_.get());
}

void GlContextImpl::initialize_vertex_inputs()
{
	vertex_input_manager_ = Ren3dGlVertexInputMgrFactory::create(this);
}

void GlContextImpl::initialize_samplers()
{
	sampler_manager_ = Ren3dGlSamplerMgrFactory::create(this);
}

void GlContextImpl::initialize_textures()
{
	texture_manager_ = Ren3dGlTextureMgrFactory::create(this);
}

void GlContextImpl::initialize_shaders()
{
	shader_manager_ = Ren3dGlShaderMgrFactory::create(this);
}

void GlContextImpl::initialize_shader_stages()
{
	shader_stage_manager_ = Ren3dGlShaderStageMgrFactory::create(this);
}

void GlContextImpl::mipmap_set_max_quality()
{
	if (!device_features_.mipmap_is_available_)
	{
		return;
	}

	if (gl_device_features_.context_kind_ == Ren3dGlContextKind::core)
	{
		return;
	}

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	Ren3dGlError::ensure_debug();
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

const Ren3dDeviceFeatures& GlContextImpl::get_device_features() const noexcept
{
	return device_features_;
}

const Ren3dGlDeviceFeatures& GlContextImpl::get_gl_device_features() const noexcept
{
	return gl_device_features_;
}

Ren3dGlBufferMgrPtr GlContextImpl::buffer_get_manager() const noexcept
{
	return buffer_manager_.get();
}

Ren3dGlSamplerMgrPtr GlContextImpl::sampler_get_manager() const noexcept
{
	return sampler_manager_.get();
}

Ren3dGlTextureMgrPtr GlContextImpl::texture_get_manager() const noexcept
{
	return texture_manager_.get();
}

Ren3dGlVaoMgrPtr GlContextImpl::vao_get_manager() const noexcept
{
	return vao_manager_.get();
}

Ren3dGlVertexInputMgrPtr GlContextImpl::vertex_input_get_manager() const noexcept
{
	return vertex_input_manager_.get();
}

Ren3dGlShaderMgrPtr GlContextImpl::shader_get_manager() const noexcept
{
	return shader_manager_.get();
}

Ren3dGlShaderStageMgrPtr GlContextImpl::shader_stage_get_manager() const noexcept
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
	const Ren3dViewport& viewport)
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
	const Ren3dScissorBox& scissor_box)
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
	const Ren3dBlendingFunc& blending_func)
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

	Ren3dGlError::ensure_debug();
}

void GlContextImpl::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	Ren3dGlError::ensure_debug();
}

void GlContextImpl::clear_set_defaults()
{
	clear_color_ = {};
	clear_set_color();
}

void GlContextImpl::viewport_set_rect()
{
	Ren3dGlUtils::viewport_set_rect(viewport_);
}

void GlContextImpl::viewport_set_depth_range()
{
	Ren3dGlUtils::viewport_set_depth_range(viewport_, gl_device_features_);
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
	Ren3dGlUtils::scissor_enable(scissor_is_enabled_);
}

void GlContextImpl::scissor_set_box()
{
	Ren3dGlUtils::scissor_set_box(scissor_box_);
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
	Ren3dGlUtils::culling_enable(culling_is_enabled_);
}

void GlContextImpl::culling_set_face()
{
	Ren3dGlUtils::culling_set_face(culling_face_);
}

void GlContextImpl::culling_set_mode()
{
	Ren3dGlUtils::culling_set_mode(culling_mode_);
}

void GlContextImpl::culling_set_defaults()
{
	culling_is_enabled_ = false;
	culling_enable();

	culling_face_ = Ren3dCullingFace::counter_clockwise;
	culling_set_face();

	culling_mode_ = Ren3dCullingMode::back;
	culling_set_mode();
}

void GlContextImpl::depth_set_test()
{
	Ren3dGlUtils::depth_test_enable(depth_is_test_enabled_);
}

void GlContextImpl::depth_set_write()
{
	Ren3dGlUtils::depth_write_enable(depth_is_write_enabled_);
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
	Ren3dGlUtils::blending_enable(blending_is_enabled_);
}

void GlContextImpl::blending_set_func()
{
	Ren3dGlUtils::blending_set_func(blending_func_);
}

void GlContextImpl::blending_set_defaults()
{
	blending_is_enabled_ = false;
	blending_enable();

	blending_func_.src_factor_ = Ren3dBlendingFactor::src_alpha;
	blending_func_.dst_factor_ = Ren3dBlendingFactor::one_minus_src_alpha;
	blending_set_func();
}

//
// GlContextImpl
// ==========================================================================


// =========================================================================
// Ren3dGlContextFactory
//

Ren3dGlContextUPtr Ren3dGlContextFactory::create(
	const Ren3dKind renderer_kind,
	const Ren3dDeviceFeatures& device_features,
	const Ren3dGlDeviceFeatures& gl_device_features)
{
	switch (renderer_kind)
	{
		case Ren3dKind::gl_2_0:
		case Ren3dKind::gl_3_2_core:
		case Ren3dKind::gles_2_0:
			return GlContextImplUPtr{new GlContextImpl{device_features, gl_device_features}};

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

//
// Ren3dGlContextFactory
// =========================================================================


} // detail
} // bstone
