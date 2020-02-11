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


#ifndef BSTONE_DETAIL_R3D_GL_CONTEXT_INCLUDED
#define BSTONE_DETAIL_R3D_GL_CONTEXT_INCLUDED


#include <memory>

#include "bstone_r3d_types.h"

#include "bstone_detail_r3d_gl_device_features.h"


namespace bstone
{
namespace detail
{


class R3dGlVao;
using R3dGlVaoPtr = R3dGlVao*;

class R3dGlBufferMgr;
using R3dGlBufferMgrPtr = R3dGlBufferMgr*;

class R3dGlSamplerMgr;
using R3dGlSamplerMgrPtr = R3dGlSamplerMgr*;

class R3dGlTextureMgr;
using R3dGlTextureMgrPtr = R3dGlTextureMgr*;

class R3dGlVaoMgr;
using R3dGlVaoMgrPtr = R3dGlVaoMgr*;

class R3dGlVertexInputMgr;
using R3dGlVertexInputMgrPtr = R3dGlVertexInputMgr*;

class R3dGlShaderMgr;
using R3dGlShaderMgrPtr = R3dGlShaderMgr*;

class R3dGlShaderStageMgr;
using R3dGlShaderStageMgrPtr = R3dGlShaderStageMgr*;


// =========================================================================
// R3dGlContext
//

class R3dGlContext
{
public:
	R3dGlContext() = default;

	virtual ~R3dGlContext() = default;


	virtual const R3dDeviceFeatures& get_device_features() const noexcept = 0;

	virtual const R3dGlDeviceFeatures& get_gl_device_features() const noexcept = 0;


	virtual R3dGlBufferMgrPtr buffer_get_manager() const noexcept = 0;

	virtual R3dGlSamplerMgrPtr sampler_get_manager() const noexcept = 0;

	virtual R3dGlTextureMgrPtr texture_get_manager() const noexcept = 0;

	virtual R3dGlVaoMgrPtr vao_get_manager() const noexcept = 0;

	virtual R3dGlVertexInputMgrPtr vertex_input_get_manager() const noexcept = 0;

	virtual R3dGlShaderMgrPtr shader_get_manager() const noexcept = 0;

	virtual R3dGlShaderStageMgrPtr shader_stage_get_manager() const noexcept = 0;


	virtual void clear(
		const Rgba8& rgba_8) = 0;


	virtual void viewport_set(
		const R3dViewport& viewport) = 0;


	virtual void scissor_enable(
		const bool is_enable) = 0;

	virtual void scissor_set_box(
		const R3dScissorBox& scissor_box) = 0;


	virtual void culling_enable(
		const bool is_enable) = 0;


	virtual void depth_test_enable(
		const bool is_enable) = 0;

	virtual void depth_write_enable(
		const bool is_enable) = 0;


	virtual void blending_enable(
		const bool is_enable) = 0;

	virtual void blending_set_func(
		const R3dBlendingFunc& func) = 0;
}; // R3dGlContext


using R3dGlContextPtr = R3dGlContext*;
using R3dGlContextUPtr = std::unique_ptr<R3dGlContext>;

//
// R3dGlContext
// =========================================================================


// =========================================================================
// R3dGlContextFactory
//


struct R3dGlContextFactory
{
	static R3dGlContextUPtr create(
		const R3dKind renderer_kind,
		const R3dDeviceFeatures& device_features,
		const R3dGlDeviceFeatures& gl_device_features);
}; // R3dGlContextFactory


//
// R3dGlContextFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_CONTEXT_INCLUDED
