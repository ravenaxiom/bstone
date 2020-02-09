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


class Renderer3dBuffer;
using Renderer3dBufferPtr = Renderer3dBuffer*;


namespace detail
{


class GlVaoManager;
using GlVaoManagerPtr = GlVaoManager*;


// ==========================================================================
// GlVao
//

class GlVao
{
protected:
	GlVao() = default;


public:
	virtual ~GlVao() = default;


	virtual void bind() = 0;


	virtual bool set_current_index_buffer(
		const Renderer3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // GlVao

using GlVaoPtr = GlVao*;
using GlVaoUPtr = std::unique_ptr<GlVao>;

//
// GlVao
// ==========================================================================


// ==========================================================================
// GlVaoDeleter
//

class GlVaoDeleter
{
public:
	GlVaoDeleter(
		const GlVaoManagerPtr gl_vao_manager);

	void operator()(
		const GlVaoPtr resource);


private:
	const GlVaoManagerPtr gl_vao_manager_;
}; // GlVaoDeleter

using GlVaoResource = std::unique_ptr<GlVao, GlVaoDeleter>;

//
// GlVaoDeleter
// ==========================================================================


// ==========================================================================
// GlVaoFactory
//

struct GlVaoFactory
{
	static GlVaoUPtr create(
		const GlVaoManagerPtr gl_vao_manager);
}; // GlVaoFactory

//
// GlVaoFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_VAO_INCLUDED
