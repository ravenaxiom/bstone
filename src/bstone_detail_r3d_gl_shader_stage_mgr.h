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
// OpenGL shader stage manager (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_SHADER_STAGE_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_SHADER_STAGE_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_shader_stage.h"


namespace bstone
{
namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;


// ==========================================================================
// R3dGlShaderStageMgr
//

class R3dGlShaderStageMgr
{
protected:
	R3dGlShaderStageMgr() = default;


public:
	virtual ~R3dGlShaderStageMgr() = default;


	virtual R3dGlContextPtr get_gl_context() const noexcept = 0;


	virtual R3dShaderStageUPtr create(
		const R3dShaderStageCreateParam& param) = 0;


	virtual void notify_destroy(
		const R3dShaderStagePtr shader_stage) noexcept = 0;


	virtual void set(
		const R3dShaderStagePtr shader_stage) = 0;


	virtual R3dShaderStagePtr get_active() const noexcept = 0;

	virtual void set_active(
		const R3dShaderStagePtr shader_stage) = 0;


	virtual R3dShaderStagePtr get_current() const noexcept = 0;

	virtual void set_current(
		const R3dShaderStagePtr shader_stage) = 0;


	virtual void set_to_current() = 0;
}; // R3dGlShaderStageMgr

using R3dGlShaderStageMgrPtr = R3dGlShaderStageMgr*;
using R3dGlShaderStageMgrUPtr = std::unique_ptr<R3dGlShaderStageMgr>;

//
// R3dGlShaderStageMgr
// ==========================================================================


// ==========================================================================
// R3dGlShaderStageMgrFactory
//

struct R3dGlShaderStageMgrFactory
{
	static R3dGlShaderStageMgrUPtr create(
		const R3dGlContextPtr gl_context);
}; // R3dGlShaderStageMgrFactory

//
// R3dGlShaderStageMgr
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_SHADER_STAGE_MGR_INCLUDED
