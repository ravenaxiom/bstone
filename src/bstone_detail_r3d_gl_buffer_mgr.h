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
// OpenGL buffer object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_REN_3D_GL_BUFFER_MGR_INCLUDED
#define BSTONE_DETAIL_REN_3D_GL_BUFFER_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_buffer.h"


namespace bstone
{
namespace detail
{


class Ren3dGlContext;
using Ren3dGlContextPtr = Ren3dGlContext*;

class Ren3dGlVaoMgr;
using Ren3dGlVaoMgrPtr = Ren3dGlVaoMgr*;


// ==========================================================================
// Ren3dGlBufferMgr
//

class Ren3dGlBufferMgr
{
protected:
	Ren3dGlBufferMgr() = default;


public:
	virtual ~Ren3dGlBufferMgr() = default;


	virtual Ren3dGlContextPtr get_context() const noexcept = 0;


	virtual Ren3dBufferUPtr buffer_create(
		const Ren3dBufferCreateParam& param) = 0;

	virtual void buffer_notify_destroy(
		const Ren3dBufferPtr buffer) noexcept = 0;


	virtual bool buffer_set_current(
		const Ren3dBufferKind buffer_kind,
		const Ren3dBufferPtr buffer) = 0;
}; // Ren3dGlBufferMgr

using Ren3dGlBufferMgrPtr = Ren3dGlBufferMgr*;
using Ren3dGlBufferMgrUPtr = std::unique_ptr<Ren3dGlBufferMgr>;

//
// Ren3dGlBufferMgr
// ==========================================================================


// ==========================================================================
// Ren3dGlBufferMgrFactory
//

struct Ren3dGlBufferMgrFactory
{
	static Ren3dGlBufferMgrUPtr create(
		const Ren3dGlContextPtr gl_context,
		const Ren3dGlVaoMgrPtr gl_vao_manager);
}; // Ren3dGlBufferMgrFactory

//
// Ren3dGlBufferMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_GL_BUFFER_MGR_INCLUDED
