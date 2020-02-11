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


#ifndef BSTONE_DETAIL_REN_3D_GL_VAO_INCLUDED
#define BSTONE_DETAIL_REN_3D_GL_VAO_INCLUDED


#include <memory>


namespace bstone
{


class Ren3dBuffer;
using Ren3dBufferPtr = Ren3dBuffer*;


namespace detail
{


class Ren3dGlVaoMgr;
using Ren3dGlVaoMgrPtr = Ren3dGlVaoMgr*;


// ==========================================================================
// Ren3dGlVao
//

class Ren3dGlVao
{
protected:
	Ren3dGlVao() = default;


public:
	virtual ~Ren3dGlVao() = default;


	virtual void bind() = 0;


	virtual bool set_current_index_buffer(
		const Ren3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // Ren3dGlVao

using Ren3dGlVaoPtr = Ren3dGlVao*;
using Ren3dGlVaoUPtr = std::unique_ptr<Ren3dGlVao>;

//
// Ren3dGlVao
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoDeleter
//

class Ren3dGlVaoDeleter
{
public:
	Ren3dGlVaoDeleter(
		const Ren3dGlVaoMgrPtr gl_vao_manager);

	void operator()(
		const Ren3dGlVaoPtr resource);


private:
	const Ren3dGlVaoMgrPtr gl_vao_manager_;
}; // Ren3dGlVaoDeleter

using Ren3dGlVaoResource = std::unique_ptr<Ren3dGlVao, Ren3dGlVaoDeleter>;

//
// Ren3dGlVaoDeleter
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoFactory
//

struct Ren3dGlVaoFactory
{
	static Ren3dGlVaoUPtr create(
		const Ren3dGlVaoMgrPtr gl_vao_manager);
}; // Ren3dGlVaoFactory

//
// Ren3dGlVaoFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_GL_VAO_INCLUDED
