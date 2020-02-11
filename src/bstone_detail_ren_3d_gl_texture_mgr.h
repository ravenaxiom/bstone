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


#ifndef BSTONE_DETAIL_REN_3D_GL_TEXTURE_MGR_INCLUDED
#define BSTONE_DETAIL_REN_3D_GL_TEXTURE_MGR_INCLUDED


#include <memory>

#include "bstone_ren_3d_sampler.h"
#include "bstone_ren_3d_texture_2d.h"


namespace bstone
{
namespace detail
{


class Ren3dGlContext;
using Ren3dGlContextPtr = Ren3dGlContext*;


// ==========================================================================
// Ren3dGlTextureMgr
//

class Ren3dGlTextureMgr
{
protected:
	Ren3dGlTextureMgr() = default;


public:
	virtual ~Ren3dGlTextureMgr() = default;


	virtual Ren3dGlContextPtr get_gl_context() const noexcept = 0;


	virtual Ren3dTexture2dUPtr create(
		const Ren3dTexture2dCreateParam& param) = 0;

	virtual void notify_destroy(
		const Ren3dTexture2dPtr texture_2d) noexcept = 0;


	virtual void set(
		const Ren3dTexture2dPtr texture_2d) = 0;


	virtual Ren3dTexture2dPtr get_active() const noexcept = 0;

	virtual void set_active(
		const Ren3dTexture2dPtr texture_2d) = 0;


	virtual Ren3dTexture2dPtr get_current() const noexcept = 0;

	virtual void set_current(
		const Ren3dTexture2dPtr texture_2d) = 0;


	virtual void set_to_current() = 0;


	virtual void update_current_sampler_state(
		const Ren3dSamplerState& sampler_state) = 0;
}; // Ren3dGlTextureMgr

using Ren3dGlTextureMgrPtr = Ren3dGlTextureMgr*;
using Ren3dGlTextureMgrUPtr = std::unique_ptr<Ren3dGlTextureMgr>;

//
// Ren3dGlTextureMgr
// ==========================================================================


// ==========================================================================
// Ren3dGlTextureMgrFactory
//

struct Ren3dGlTextureMgrFactory
{
	static Ren3dGlTextureMgrUPtr create(
		const Ren3dGlContextPtr gl_context);
}; // Ren3dGlTextureMgrFactory

//
// Ren3dGlTextureMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_GL_TEXTURE_MGR_INCLUDED
