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
// 3D renderer's shader variable.
//


#ifndef BSTONE_R3D_SHADER_VAR_INCLUDED
#define BSTONE_R3D_SHADER_VAR_INCLUDED


#include <memory>
#include <string>


namespace bstone
{


// ==========================================================================
// Renderer3dShaderVar
//

enum class Renderer3dShaderVarKind
{
	none,
	attribute,
	sampler,
	uniform,
}; // Renderer3dShaderVarKind

enum class Renderer3dShaderVarTypeId
{
	none,
	int32,
	float32,
	vec2,
	vec3,
	vec4,
	mat4,
	sampler2d,
}; // Renderer3dShaderVarTypeId


class Renderer3dShaderVar
{
protected:
	Renderer3dShaderVar() = default;

	virtual ~Renderer3dShaderVar() = default;


public:
	virtual Renderer3dShaderVarKind get_kind() const noexcept = 0;

	virtual Renderer3dShaderVarTypeId get_type_id() const noexcept = 0;

	virtual int get_index() const noexcept = 0;

	virtual const std::string& get_name() const noexcept = 0;

	virtual int get_input_index() const noexcept = 0;
}; // Renderer3dShaderVar

using Renderer3dShaderVarPtr = Renderer3dShaderVar*;
using Renderer3dShaderVarCPtr = const Renderer3dShaderVar*;

//
// Renderer3dShaderVar
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarInt32
//

class Renderer3dShaderVarInt32 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarInt32() = default;

	virtual ~Renderer3dShaderVarInt32() = default;


public:
	virtual void set_int(
		const std::int32_t value) = 0;
}; // Renderer3dShaderVarInt32

using Renderer3dShaderVarInt32Ptr = Renderer3dShaderVarInt32*;
using Renderer3dShaderVarInt32CPtr = const Renderer3dShaderVarInt32*;

//
// Renderer3dShaderVarInt32
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarFloat32
//

class Renderer3dShaderVarFloat32 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarFloat32() = default;

	virtual ~Renderer3dShaderVarFloat32() = default;


public:
	virtual void set_float(
		const float value) = 0;
}; // Renderer3dShaderVarFloat32

using Renderer3dShaderVarFloat32Ptr = Renderer3dShaderVarFloat32*;
using Renderer3dShaderVarFloat32CPtr = const Renderer3dShaderVarFloat32*;

//
// Renderer3dShaderVarFloat32
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarVec2
//

class Renderer3dShaderVarVec2 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarVec2() = default;

	virtual ~Renderer3dShaderVarVec2() = default;


public:
	virtual void set_vec2(
		const float* const value) = 0;
}; // Renderer3dShaderVarVec2

using Renderer3dShaderVarVec2Ptr = Renderer3dShaderVarVec2*;
using Renderer3dShaderVarVec2CPtr = const Renderer3dShaderVarVec2*;

//
// Renderer3dShaderVarVec2
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarVec4
//

class Renderer3dShaderVarVec4 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarVec4() = default;

	virtual ~Renderer3dShaderVarVec4() = default;


public:
	virtual void set_vec4(
		const float* const value) = 0;
}; // Renderer3dShaderVarVec4

using Renderer3dShaderVarVec4Ptr = Renderer3dShaderVarVec4*;
using Renderer3dShaderVarVec4CPtr = const Renderer3dShaderVarVec4*;

//
// Renderer3dShaderVarVec4
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarMat4
//

class Renderer3dShaderVarMat4 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarMat4() = default;

	virtual ~Renderer3dShaderVarMat4() = default;


public:
	virtual void set_mat4(
		const float* const value) = 0;
}; // Renderer3dShaderVarMat4

using Renderer3dShaderVarMat4Ptr = Renderer3dShaderVarMat4*;
using Renderer3dShaderVarMat4CPtr = const Renderer3dShaderVarMat4*;

//
// Renderer3dShaderVarMat4
// ==========================================================================


// ==========================================================================
// Renderer3dShaderVarSampler2d
//

class Renderer3dShaderVarSampler2d :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarSampler2d() = default;

	virtual ~Renderer3dShaderVarSampler2d() = default;


public:
	virtual void set_sampler_2d(
		const std::int32_t value) = 0;
}; // Renderer3dShaderVarSampler2d

using Renderer3dShaderVarSampler2dPtr = Renderer3dShaderVarSampler2d*;
using Renderer3dShaderVarSampler2dCPtr = const Renderer3dShaderVarSampler2d*;

//
// Renderer3dShaderVarSampler2d
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SHADER_VAR_INCLUDED
