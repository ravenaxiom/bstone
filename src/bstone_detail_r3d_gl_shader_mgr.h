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
// OpenGL shader object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_SHADER_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_SHADER_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_shader.h"


namespace bstone
{
namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;


// ==========================================================================
// R3dGlShaderMgr
//

class R3dGlShaderMgr
{
protected:
	R3dGlShaderMgr() = default;


public:
	virtual ~R3dGlShaderMgr() = default;


	virtual R3dShaderUPtr create(
		const R3dShaderCreateParam& param) = 0;

	virtual void notify_destroy(
		const R3dShaderPtr shader) noexcept = 0;
}; // R3dGlShaderMgr

using R3dGlShaderMgrPtr = R3dGlShaderMgr*;
using R3dGlShaderMgrUPtr = std::unique_ptr<R3dGlShaderMgr>;

//
// R3dGlShaderMgr
// ==========================================================================


// ==========================================================================
// R3dGlShaderMgrFactory
//

struct R3dGlShaderMgrFactory
{
	static R3dGlShaderMgrUPtr create(
		const R3dGlContextPtr gl_context);
}; // R3dGlShaderMgrFactory

//
// R3dGlShaderMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_SHADER_MGR_INCLUDED
