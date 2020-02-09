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
// 3D renderer's vertex input.
//


#ifndef BSTONE_RENDERER_3D_VERTEX_INPUT_INCLUDED
#define BSTONE_RENDERER_3D_VERTEX_INPUT_INCLUDED


#include <memory>
#include <vector>

#include "bstone_r3d_buffer.h"
#include "bstone_r3d_types.h"


namespace bstone
{


// ==========================================================================
// Renderer3dVertexInput
//

enum class Renderer3dVertexAttributeFormat
{
	none,
	rgba_8_unorm,
	rg_32_sfloat,
	rgb_32_sfloat,
}; // Renderer3dVertexAttributeFormat

struct Renderer3dVertexAttributeDescription
{
	bool is_default_;
	int location_;
	Renderer3dVertexAttributeFormat format_;
	Renderer3dBufferPtr vertex_buffer_;
	int offset_;
	int stride_;
	Renderer3dVec4 default_value_;
}; // Renderer3dVertexAttributeDescription

using Renderer3dVertexAttributeDescriptions = std::vector<Renderer3dVertexAttributeDescription>;

struct Renderer3dVertexInputCreateParam
{
	Renderer3dBufferPtr index_buffer_;
	Renderer3dVertexAttributeDescriptions attribute_descriptions_;
}; // Renderer3dVertexInputCreateParam


class Renderer3dVertexInput
{
protected:
	Renderer3dVertexInput() = default;


public:
	virtual ~Renderer3dVertexInput() = default;
}; // Renderer3dVertexInput

using Renderer3dVertexInputPtr = Renderer3dVertexInput*;
using Renderer3dVertexInputUPtr = std::unique_ptr<Renderer3dVertexInput>;

//
// Renderer3dVertexInput
// ==========================================================================


} // bstone


#endif // !BSTONE_RENDERER_3D_VERTEX_INPUT_INCLUDED
