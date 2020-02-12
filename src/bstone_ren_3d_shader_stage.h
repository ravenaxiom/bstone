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
// 3D renderer's shader stage.
//


#ifndef BSTONE_REN_3D_SHADER_STAGE_INCLUDED
#define BSTONE_REN_3D_SHADER_STAGE_INCLUDED


#include <memory>
#include <string>
#include <vector>

#include "bstone_ren_3d_shader.h"
#include "bstone_ren_3d_shader_stage.h"
#include "bstone_ren_3d_shader_var.h"


namespace bstone
{


// ==========================================================================
// Ren3dShaderStage
//

struct Ren3dShaderStageInputBinding
{
	int index_;
	std::string name_;
}; // Ren3dShaderStageInputBinding

using Ren3dShaderStageInputBindings = std::vector<Ren3dShaderStageInputBinding>;


struct Ren3dCreateShaderStageParam
{
	Ren3dShaderPtr fragment_shader_;
	Ren3dShaderPtr vertex_shader_;
	Ren3dShaderStageInputBindings input_bindings_;
}; // Ren3dCreateShaderStageParam


class Ren3dShaderStage
{
protected:
	Ren3dShaderStage() = default;


public:
	virtual ~Ren3dShaderStage() = default;


	virtual Ren3dShaderVarPtr find_var(
		const std::string& name) = 0;

	virtual Ren3dShaderVarInt32Ptr find_var_int32(
		const std::string& name) = 0;

	virtual Ren3dShaderVarFloat32Ptr find_var_float32(
		const std::string& name) = 0;

	virtual Ren3dShaderVarVec2Ptr find_var_vec2(
		const std::string& name) = 0;

	virtual Ren3dShaderVarVec4Ptr find_var_vec4(
		const std::string& name) = 0;

	virtual Ren3dShaderVarMat4Ptr find_var_mat4(
		const std::string& name) = 0;

	virtual Ren3dShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) = 0;
}; // Ren3dShaderStage

using Ren3dShaderStagePtr = Ren3dShaderStage*;
using Ren3dShaderStageUPtr = std::unique_ptr<Ren3dShaderStage>;

//
// Ren3dShaderStage
// ==========================================================================


} // bstone


#endif // !BSTONE_REN_3D_SHADER_STAGE_INCLUDED
