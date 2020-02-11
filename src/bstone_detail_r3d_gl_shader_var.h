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
// OpenGL shader variable (implementation).
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_DETAIL_R3D_GL_SHADER_VAR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_SHADER_VAR_INCLUDED


#include "bstone_r3d_shader_var.h"


namespace bstone
{
namespace detail
{


class R3dGlShaderStage;
using R3dGlShaderStagePtr = R3dGlShaderStage*;


// ==========================================================================
// R3dGlShaderVar
//

class R3dGlShaderVar :
	public virtual R3dShaderVarInt32,
	public virtual R3dShaderVarFloat32,
	public virtual R3dShaderVarVec2,
	public virtual R3dShaderVarVec4,
	public virtual R3dShaderVarMat4,
	public virtual R3dShaderVarSampler2d
{
protected:
	R3dGlShaderVar() = default;


public:
	~R3dGlShaderVar() override = default;


	static int get_unit_size(
		const R3dShaderVarTypeId type_id);
}; // R3dGlShaderVar

using R3dGlShaderVarPtr = R3dGlShaderVar*;
using R3dGlShaderVarUPtr = std::unique_ptr<R3dGlShaderVar>;

//
// R3dGlShaderVar
// ==========================================================================


// ==========================================================================
// R3dGlShaderVarFactory
//

struct R3dGlShaderVarFactory
{
	struct CreateParam
	{
		R3dShaderVarKind kind_;
		R3dShaderVarTypeId type_id_;
		int value_size_;
		int index_;
		std::string name_;
		int input_index_;
		int gl_location_;
	}; // CreateParam

	static R3dGlShaderVarUPtr create(
		const R3dGlShaderStagePtr shader_stage,
		const CreateParam& param);
}; // R3dGlShaderVarFactory

//
// R3dGlShaderVarFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_SHADER_VAR_INCLUDED
