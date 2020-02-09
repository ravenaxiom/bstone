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


#ifndef BSTONE_RENDERER_3D_SHADER_STAGE_INCLUDED
#define BSTONE_RENDERER_3D_SHADER_STAGE_INCLUDED


#include <memory>
#include <string>
#include <vector>

#include "bstone_r3d_shader.h"
#include "bstone_r3d_shader_stage.h"
#include "bstone_r3d_shader_var.h"


namespace bstone
{


// ==========================================================================
// Renderer3dShaderStage
//

struct Renderer3dShaderStageInputBinding
{
	int index_;
	std::string name_;
}; // Renderer3dShaderStageInputBinding

using Renderer3dShaderStageInputBindings = std::vector<Renderer3dShaderStageInputBinding>;


struct Renderer3dShaderStageCreateParam
{
	Renderer3dShaderPtr fragment_shader_;
	Renderer3dShaderPtr vertex_shader_;
	Renderer3dShaderStageInputBindings input_bindings_;
}; // Renderer3dShaderStageCreateParam


class Renderer3dShaderStage
{
protected:
	Renderer3dShaderStage() = default;


public:
	virtual ~Renderer3dShaderStage() = default;


	virtual Renderer3dShaderVarPtr find_var(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarInt32Ptr find_var_int32(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarFloat32Ptr find_var_float32(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarVec2Ptr find_var_vec2(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarVec4Ptr find_var_vec4(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarMat4Ptr find_var_mat4(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) = 0;
}; // Renderer3dShaderStage

using Renderer3dShaderStagePtr = Renderer3dShaderStage*;
using Renderer3dShaderStageUPtr = std::unique_ptr<Renderer3dShaderStage>;

//
// Renderer3dShaderStage
// ==========================================================================


} // bstone


#endif // !BSTONE_RENDERER_3D_SHADER_STAGE_INCLUDED
