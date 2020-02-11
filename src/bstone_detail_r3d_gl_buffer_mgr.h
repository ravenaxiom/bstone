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


#ifndef BSTONE_DETAIL_R3D_GL_BUFFER_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_BUFFER_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_buffer.h"


namespace bstone
{
namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;

class R3dGlVaoMgr;
using R3dGlVaoMgrPtr = R3dGlVaoMgr*;


// ==========================================================================
// R3dGlBufferMgr
//

class R3dGlBufferMgr
{
protected:
	R3dGlBufferMgr() = default;


public:
	virtual ~R3dGlBufferMgr() = default;


	virtual R3dGlContextPtr get_context() const noexcept = 0;


	virtual R3dBufferUPtr buffer_create(
		const R3dBufferCreateParam& param) = 0;

	virtual void buffer_notify_destroy(
		const R3dBufferPtr buffer) noexcept = 0;


	virtual bool buffer_set_current(
		const R3dBufferKind buffer_kind,
		const R3dBufferPtr buffer) = 0;
}; // R3dGlBufferMgr

using R3dGlBufferMgrPtr = R3dGlBufferMgr*;
using R3dGlBufferMgrUPtr = std::unique_ptr<R3dGlBufferMgr>;

//
// R3dGlBufferMgr
// ==========================================================================


// ==========================================================================
// R3dGlBufferMgrFactory
//

struct R3dGlBufferMgrFactory
{
	static R3dGlBufferMgrUPtr create(
		const R3dGlContextPtr gl_context,
		const R3dGlVaoMgrPtr gl_vao_manager);
}; // R3dGlBufferMgrFactory

//
// R3dGlBufferMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_BUFFER_MGR_INCLUDED
