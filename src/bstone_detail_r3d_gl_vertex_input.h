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
// OpenGL vertex input (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_VERTEX_INPUT_INCLUDED
#define BSTONE_DETAIL_R3D_GL_VERTEX_INPUT_INCLUDED


#include "bstone_r3d_vertex_input.h"

#include "bstone_detail_r3d_gl_device_features.h"


namespace bstone
{
namespace detail
{


class R3dGlVertexInputMgr;
using R3dGlVertexInputMgrPtr = R3dGlVertexInputMgr*;


// =========================================================================
// R3dGlVertexInput
//

class R3dGlVertexInput :
	public R3dVertexInput
{
protected:
	R3dGlVertexInput() = default;


public:
	~R3dGlVertexInput() override = default;


	virtual void bind() = 0;

	virtual R3dBufferPtr get_index_buffer() const noexcept = 0;
}; // VertexInput

using R3dGlVertexInputPtr = R3dGlVertexInput*;
using R3dGlVertexInputUPtr = std::unique_ptr<R3dGlVertexInput>;

//
// R3dGlVertexInput
// =========================================================================


// =========================================================================
// R3dGlVertexInputFactory
//

struct R3dGlVertexInputFactory
{
	static R3dGlVertexInputUPtr create(
		const R3dGlVertexInputMgrPtr vertex_input_manager,
		const R3dVertexInputCreateParam& param);
}; // R3dGlVertexInputFactory

//
// R3dGlVertexInputFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_VERTEX_INPUT_INCLUDED
