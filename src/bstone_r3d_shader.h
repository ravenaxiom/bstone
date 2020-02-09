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

enum class Renderer3dShaderKind
{
	none,
	fragment,
	vertex,
}; // Renderer3dShaderKind

struct Renderer3dShaderSource
{
	const void* data_;
	int size_;
}; // Source

struct Renderer3dShaderCreateParam
{
	Renderer3dShaderKind kind_;
	Renderer3dShaderSource source_;
}; // CreateParam

class Renderer3dShader
{
protected:
	Renderer3dShader() = default;


public:
	virtual ~Renderer3dShader() = default;


	virtual Renderer3dShaderKind get_kind() const noexcept = 0;
}; // Renderer3dShader

using Renderer3dShaderPtr = Renderer3dShader*;
using Renderer3dShaderUPtr = std::unique_ptr<Renderer3dShader>;

//
// Shader
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SHADER_INCLUDED
