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


#ifndef BSTONE_R3D_SHADER_STAGE_INCLUDED
#define BSTONE_R3D_SHADER_STAGE_INCLUDED


#include <memory>
#include <string>
#include <vector>

#include "bstone_r3d_shader.h"
#include "bstone_r3d_shader_stage.h"
#include "bstone_r3d_shader_var.h"


namespace bstone
{


// ==========================================================================
// R3dShaderStage
//

struct R3dShaderStageInputBinding
{
	int index_;
	std::string name_;
}; // R3dShaderStageInputBinding

using R3dShaderStageInputBindings = std::vector<R3dShaderStageInputBinding>;


struct R3dShaderStageCreateParam
{
	R3dShaderPtr fragment_shader_;
	R3dShaderPtr vertex_shader_;
	R3dShaderStageInputBindings input_bindings_;
}; // R3dShaderStageCreateParam


class R3dShaderStage
{
protected:
	R3dShaderStage() = default;


public:
	virtual ~R3dShaderStage() = default;


	virtual R3dShaderVarPtr find_var(
		const std::string& name) = 0;

	virtual R3dShaderVarInt32Ptr find_var_int32(
		const std::string& name) = 0;

	virtual R3dShaderVarFloat32Ptr find_var_float32(
		const std::string& name) = 0;

	virtual R3dShaderVarVec2Ptr find_var_vec2(
		const std::string& name) = 0;

	virtual R3dShaderVarVec4Ptr find_var_vec4(
		const std::string& name) = 0;

	virtual R3dShaderVarMat4Ptr find_var_mat4(
		const std::string& name) = 0;

	virtual R3dShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) = 0;
}; // R3dShaderStage

using R3dShaderStagePtr = R3dShaderStage*;
using R3dShaderStageUPtr = std::unique_ptr<R3dShaderStage>;

//
// R3dShaderStage
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SHADER_STAGE_INCLUDED
