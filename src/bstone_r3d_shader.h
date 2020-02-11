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
// 3D renderer's shader.
//


#ifndef BSTONE_R3D_SHADER_INCLUDED
#define BSTONE_R3D_SHADER_INCLUDED


#include <memory>


namespace bstone
{


// ==========================================================================
// Shader
//

enum class R3dShaderKind
{
	none,
	fragment,
	vertex,
}; // R3dShaderKind

struct R3dShaderSource
{
	const void* data_;
	int size_;
}; // Source

struct R3dShaderCreateParam
{
	R3dShaderKind kind_;
	R3dShaderSource source_;
}; // CreateParam

class R3dShader
{
protected:
	R3dShader() = default;


public:
	virtual ~R3dShader() = default;


	virtual R3dShaderKind get_kind() const noexcept = 0;
}; // R3dShader

using R3dShaderPtr = R3dShader*;
using R3dShaderUPtr = std::unique_ptr<R3dShader>;

//
// Shader
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SHADER_INCLUDED
