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
// R3dShaderVar
//

enum class R3dShaderVarKind
{
	none,
	attribute,
	sampler,
	uniform,
}; // R3dShaderVarKind

enum class R3dShaderVarTypeId
{
	none,
	int32,
	float32,
	vec2,
	vec3,
	vec4,
	mat4,
	sampler2d,
}; // R3dShaderVarTypeId


class R3dShaderVar
{
protected:
	R3dShaderVar() = default;

	virtual ~R3dShaderVar() = default;


public:
	virtual R3dShaderVarKind get_kind() const noexcept = 0;

	virtual R3dShaderVarTypeId get_type_id() const noexcept = 0;

	virtual int get_index() const noexcept = 0;

	virtual const std::string& get_name() const noexcept = 0;

	virtual int get_input_index() const noexcept = 0;
}; // R3dShaderVar

using R3dShaderVarPtr = R3dShaderVar*;
using R3dShaderVarCPtr = const R3dShaderVar*;

//
// R3dShaderVar
// ==========================================================================


// ==========================================================================
// R3dShaderVarInt32
//

class R3dShaderVarInt32 :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarInt32() = default;

	virtual ~R3dShaderVarInt32() = default;


public:
	virtual void set_int(
		const std::int32_t value) = 0;
}; // R3dShaderVarInt32

using R3dShaderVarInt32Ptr = R3dShaderVarInt32*;
using R3dShaderVarInt32CPtr = const R3dShaderVarInt32*;

//
// R3dShaderVarInt32
// ==========================================================================


// ==========================================================================
// R3dShaderVarFloat32
//

class R3dShaderVarFloat32 :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarFloat32() = default;

	virtual ~R3dShaderVarFloat32() = default;


public:
	virtual void set_float(
		const float value) = 0;
}; // R3dShaderVarFloat32

using R3dShaderVarFloat32Ptr = R3dShaderVarFloat32*;
using R3dShaderVarFloat32CPtr = const R3dShaderVarFloat32*;

//
// R3dShaderVarFloat32
// ==========================================================================


// ==========================================================================
// R3dShaderVarVec2
//

class R3dShaderVarVec2 :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarVec2() = default;

	virtual ~R3dShaderVarVec2() = default;


public:
	virtual void set_vec2(
		const float* const value) = 0;
}; // R3dShaderVarVec2

using R3dShaderVarVec2Ptr = R3dShaderVarVec2*;
using R3dShaderVarVec2CPtr = const R3dShaderVarVec2*;

//
// R3dShaderVarVec2
// ==========================================================================


// ==========================================================================
// R3dShaderVarVec4
//

class R3dShaderVarVec4 :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarVec4() = default;

	virtual ~R3dShaderVarVec4() = default;


public:
	virtual void set_vec4(
		const float* const value) = 0;
}; // R3dShaderVarVec4

using R3dShaderVarVec4Ptr = R3dShaderVarVec4*;
using R3dShaderVarVec4CPtr = const R3dShaderVarVec4*;

//
// R3dShaderVarVec4
// ==========================================================================


// ==========================================================================
// R3dShaderVarMat4
//

class R3dShaderVarMat4 :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarMat4() = default;

	virtual ~R3dShaderVarMat4() = default;


public:
	virtual void set_mat4(
		const float* const value) = 0;
}; // R3dShaderVarMat4

using R3dShaderVarMat4Ptr = R3dShaderVarMat4*;
using R3dShaderVarMat4CPtr = const R3dShaderVarMat4*;

//
// R3dShaderVarMat4
// ==========================================================================


// ==========================================================================
// R3dShaderVarSampler2d
//

class R3dShaderVarSampler2d :
	public virtual R3dShaderVar
{
protected:
	R3dShaderVarSampler2d() = default;

	virtual ~R3dShaderVarSampler2d() = default;


public:
	virtual void set_sampler_2d(
		const std::int32_t value) = 0;
}; // R3dShaderVarSampler2d

using R3dShaderVarSampler2dPtr = R3dShaderVarSampler2d*;
using R3dShaderVarSampler2dCPtr = const R3dShaderVarSampler2d*;

//
// R3dShaderVarSampler2d
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SHADER_VAR_INCLUDED
