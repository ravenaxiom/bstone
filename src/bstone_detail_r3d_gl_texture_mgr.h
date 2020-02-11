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
// OpenGL texture object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_TEXTURE_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_TEXTURE_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_sampler.h"
#include "bstone_r3d_texture_2d.h"


namespace bstone
{
namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;


// ==========================================================================
// R3dGlTextureMgr
//

class R3dGlTextureMgr
{
protected:
	R3dGlTextureMgr() = default;


public:
	virtual ~R3dGlTextureMgr() = default;


	virtual R3dGlContextPtr get_gl_context() const noexcept = 0;


	virtual R3dTexture2dUPtr create(
		const R3dTexture2dCreateParam& param) = 0;

	virtual void notify_destroy(
		const R3dTexture2dPtr texture_2d) noexcept = 0;


	virtual void set(
		const R3dTexture2dPtr texture_2d) = 0;


	virtual R3dTexture2dPtr get_active() const noexcept = 0;

	virtual void set_active(
		const R3dTexture2dPtr texture_2d) = 0;


	virtual R3dTexture2dPtr get_current() const noexcept = 0;

	virtual void set_current(
		const R3dTexture2dPtr texture_2d) = 0;


	virtual void set_to_current() = 0;


	virtual void update_current_sampler_state(
		const R3dSamplerState& sampler_state) = 0;
}; // R3dGlTextureMgr

using R3dGlTextureMgrPtr = R3dGlTextureMgr*;
using R3dGlTextureMgrUPtr = std::unique_ptr<R3dGlTextureMgr>;

//
// R3dGlTextureMgr
// ==========================================================================


// ==========================================================================
// R3dGlTextureMgrFactory
//

struct R3dGlTextureMgrFactory
{
	static R3dGlTextureMgrUPtr create(
		const R3dGlContextPtr gl_context);
}; // R3dGlTextureMgrFactory

//
// R3dGlTextureMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_TEXTURE_MGR_INCLUDED
