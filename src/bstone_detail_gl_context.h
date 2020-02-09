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
// OpenGL context.
//


#ifndef BSTONE_DETAIL_GL_CONTEXT_INCLUDED
#define BSTONE_DETAIL_GL_CONTEXT_INCLUDED


#include <memory>

#include "bstone_renderer_3d_types.h"

#include "bstone_detail_gl_device_features.h"


namespace bstone
{
namespace detail
{


class GlVao;
using GlVaoPtr = GlVao*;

class GlBufferManager;
using GlBufferManagerPtr = GlBufferManager*;

class GlSamplerManager;
using GlSamplerManagerPtr = GlSamplerManager*;

class GlTextureManager;
using GlTextureManagerPtr = GlTextureManager*;

class GlVaoManager;
using GlVaoManagerPtr = GlVaoManager*;

class GlVertexInputManager;
using GlVertexInputManagerPtr = GlVertexInputManager*;

class GlShaderManager;
using GlShaderManagerPtr = GlShaderManager*;

class GlShaderStageManager;
using GlShaderStageManagerPtr = GlShaderStageManager*;


// =========================================================================
// GlContext
//

class GlContext
{
public:
	GlContext() = default;

	virtual ~GlContext() = default;


	virtual const Renderer3dDeviceFeatures& get_device_features() const noexcept = 0;

	virtual const GlDeviceFeatures& get_gl_device_features() const noexcept = 0;


	virtual GlBufferManagerPtr buffer_get_manager() const noexcept = 0;

	virtual GlSamplerManagerPtr sampler_get_manager() const noexcept = 0;

	virtual GlTextureManagerPtr texture_get_manager() const noexcept = 0;

	virtual GlVaoManagerPtr vao_get_manager() const noexcept = 0;

	virtual GlVertexInputManagerPtr vertex_input_get_manager() const noexcept = 0;

	virtual GlShaderManagerPtr shader_get_manager() const noexcept = 0;

	virtual GlShaderStageManagerPtr shader_stage_get_manager() const noexcept = 0;


	virtual void clear(
		const Rgba8& rgba_8) = 0;


	virtual void viewport_set(
		const Renderer3dViewport& viewport) = 0;


	virtual void scissor_enable(
		const bool is_enable) = 0;

	virtual void scissor_set_box(
		const Renderer3dScissorBox& scissor_box) = 0;


	virtual void culling_enable(
		const bool is_enable) = 0;


	virtual void depth_test_enable(
		const bool is_enable) = 0;

	virtual void depth_write_enable(
		const bool is_enable) = 0;


	virtual void blending_enable(
		const bool is_enable) = 0;

	virtual void blending_set_func(
		const Renderer3dBlendingFunc& func) = 0;
}; // GlContext


using GlContextPtr = GlContext*;
using GlContextUPtr = std::unique_ptr<GlContext>;

//
// GlContext
// =========================================================================


// =========================================================================
// GlContextFactory
//


struct GlContextFactory
{
	static GlContextUPtr create(
		const Renderer3dKind renderer_kind,
		const Renderer3dDeviceFeatures& device_features,
		const GlDeviceFeatures& gl_device_features);
}; // GlContextFactory


//
// GlContextFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_CONTEXT_INCLUDED
