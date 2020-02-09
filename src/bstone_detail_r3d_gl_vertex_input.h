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


class GlVertexInputManager;
using GlVertexInputManagerPtr = GlVertexInputManager*;


// =========================================================================
// GlVertexInput
//

class GlVertexInput :
	public Renderer3dVertexInput
{
protected:
	GlVertexInput() = default;


public:
	~GlVertexInput() override = default;


	virtual void bind() = 0;

	virtual Renderer3dBufferPtr get_index_buffer() const noexcept = 0;
}; // VertexInput

using GlVertexInputPtr = GlVertexInput*;
using GlVertexInputUPtr = std::unique_ptr<GlVertexInput>;

//
// GlVertexInput
// =========================================================================


// =========================================================================
// GlVertexInputFactory
//

struct GlVertexInputFactory
{
	static GlVertexInputUPtr create(
		const GlVertexInputManagerPtr vertex_input_manager,
		const Renderer3dVertexInputCreateParam& param);
}; // GlVertexInputFactory

//
// GlVertexInputFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_VERTEX_INPUT_INCLUDED
