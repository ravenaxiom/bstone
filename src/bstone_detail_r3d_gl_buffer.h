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
// OpenGL buffer object (implementation).
//
// !!! Internal usage only !!!
//


#ifndef BSTONE_DETAIL_R3D_GL_BUFFER_INCLUDED
#define BSTONE_DETAIL_R3D_GL_BUFFER_INCLUDED


#include "bstone_r3d_buffer.h"


namespace bstone
{
namespace detail
{


class R3dGlBufferMgr;
using R3dGlBufferMgrPtr = R3dGlBufferMgr*;


// =========================================================================
// R3dGlBuffer
//

class R3dGlBuffer :
	public R3dBuffer
{
protected:
	R3dGlBuffer() = default;


public:
	~R3dGlBuffer() override = default;


	virtual void set(
		const bool is_set) = 0;
}; // R3dGlBuffer


using R3dGlBufferPtr = R3dGlBuffer*;
using R3dGlBufferUPtr = std::unique_ptr<R3dGlBuffer>;

//
// R3dGlBuffer
// =========================================================================


// =========================================================================
// R3dGlBufferFactory
//

struct R3dGlBufferFactory
{
	static R3dGlBufferUPtr create(
		const R3dGlBufferMgrPtr gl_buffer_manager,
		const R3dBufferCreateParam& param);
}; // R3dGlBufferFactory

//
// R3dGlBufferFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_BUFFER_INCLUDED
