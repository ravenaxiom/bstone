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
// OpenGL vertex array object (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_VAO_INCLUDED
#define BSTONE_DETAIL_R3D_GL_VAO_INCLUDED


#include <memory>


namespace bstone
{


class R3dBuffer;
using R3dBufferPtr = R3dBuffer*;


namespace detail
{


class R3dGlVaoMgr;
using R3dGlVaoMgrPtr = R3dGlVaoMgr*;


// ==========================================================================
// R3dGlVao
//

class R3dGlVao
{
protected:
	R3dGlVao() = default;


public:
	virtual ~R3dGlVao() = default;


	virtual void bind() = 0;


	virtual bool set_current_index_buffer(
		const R3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // R3dGlVao

using R3dGlVaoPtr = R3dGlVao*;
using R3dGlVaoUPtr = std::unique_ptr<R3dGlVao>;

//
// R3dGlVao
// ==========================================================================


// ==========================================================================
// R3dGlVaoDeleter
//

class R3dGlVaoDeleter
{
public:
	R3dGlVaoDeleter(
		const R3dGlVaoMgrPtr gl_vao_manager);

	void operator()(
		const R3dGlVaoPtr resource);


private:
	const R3dGlVaoMgrPtr gl_vao_manager_;
}; // R3dGlVaoDeleter

using R3dGlVaoResource = std::unique_ptr<R3dGlVao, R3dGlVaoDeleter>;

//
// R3dGlVaoDeleter
// ==========================================================================


// ==========================================================================
// R3dGlVaoFactory
//

struct R3dGlVaoFactory
{
	static R3dGlVaoUPtr create(
		const R3dGlVaoMgrPtr gl_vao_manager);
}; // R3dGlVaoFactory

//
// R3dGlVaoFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_VAO_INCLUDED
