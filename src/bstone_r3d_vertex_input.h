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


#ifndef BSTONE_R3D_VERTEX_INPUT_INCLUDED
#define BSTONE_R3D_VERTEX_INPUT_INCLUDED


#include <memory>
#include <vector>

#include "bstone_r3d_buffer.h"
#include "bstone_r3d_types.h"


namespace bstone
{


// ==========================================================================
// R3dVertexInput
//

enum class R3dVertexAttribFormat
{
	none,
	rgba_8_unorm,
	rg_32_sfloat,
	rgb_32_sfloat,
}; // R3dVertexAttribFormat

struct R3dVertexAttributeDescription
{
	bool is_default_;
	int location_;
	R3dVertexAttribFormat format_;
	R3dBufferPtr vertex_buffer_;
	int offset_;
	int stride_;
	R3dVec4 default_value_;
}; // R3dVertexAttributeDescription

using R3dVertexAttributeDescriptions = std::vector<R3dVertexAttributeDescription>;

struct R3dVertexInputCreateParam
{
	R3dBufferPtr index_buffer_;
	R3dVertexAttributeDescriptions attribute_descriptions_;
}; // R3dVertexInputCreateParam


class R3dVertexInput
{
protected:
	R3dVertexInput() = default;


public:
	virtual ~R3dVertexInput() = default;
}; // R3dVertexInput

using R3dVertexInputPtr = R3dVertexInput*;
using R3dVertexInputUPtr = std::unique_ptr<R3dVertexInput>;

//
// R3dVertexInput
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_VERTEX_INPUT_INCLUDED
