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
// 3D-renderer's command buffer.
//


#ifndef BSTONE_REN_3D_COMMAND_BUFFER_INCLUDED
#define BSTONE_REN_3D_COMMAND_BUFFER_INCLUDED


#include "bstone_ren_3d.h"


namespace bstone
{


enum class Ren3dCmdId
{
	none,

	clear,

	viewport,

	scissor,
	scissor_set_box,

	culling,

	depth_set_test,
	depth_set_write,

	blending,
	blending_func,

	texture,
	sampler,

	vertex_input,

	shader_stage,

	shader_var_int32,
	shader_var_float32,
	shader_var_vec2,
	shader_var_vec4,
	shader_var_mat4,
	shader_var_sampler_2d,

	draw_indexed,
}; // Ren3dCmdId


struct Ren3dClearCmd
{
	Ren3dClearParam clear_;
}; // Ren3dClearCmd

struct Ren3dSetViewportCmd
{
	Ren3dViewport viewport_;
}; // Ren3dSetViewportCmd

struct Ren3dEnableScissorCmd
{
	bool is_enable_;
}; // Ren3dEnableScissorCmd

struct Ren3dSetScissorBoxCmd
{
	Ren3dScissorBox scissor_box_;
}; // Ren3dSetScissorBoxCmd

struct Ren3dEnableCullingCmd
{
	bool is_enable_;
}; // Ren3dEnableCullingCmd

struct Ren3dEnableBlendingCmd
{
	bool is_enable_;
}; // Ren3dEnableBlendingCmd

struct Ren3dSetBlendingFuncCmd
{
	Ren3dBlendingFunc blending_func_;
}; // Ren3dSetBlendingFuncCmd

struct Ren3dEnableDepthTestCmd
{
	bool is_enable_;
}; // Ren3dEnableDepthTestCmd

struct Ren3dEnableDepthWriteCmd
{
	bool is_enable_;
}; // Ren3dEnableDepthWriteCmd

struct Ren3dSetTextureCmd
{
	Ren3dTexture2dPtr texture_2d_;
}; // Ren3dSetTextureCmd

struct Ren3dSetSamplerCmd
{
	Ren3dSamplerPtr sampler_;
}; // Ren3dSetSamplerCmd

struct Ren3dSetVertexInputCmd
{
	Ren3dVertexInputPtr vertex_input_;
}; // Ren3dSetVertexInputCmd

struct Ren3dSetShaderStageCmd
{
	Ren3dShaderStagePtr shader_stage_;
}; // Ren3dSetShaderStageCmd

struct Ren3dSetInt32UniformCmd
{
	Ren3dShaderVarInt32Ptr var_;
	int value_;
}; // Ren3dSetInt32UniformCmd

struct Ren3dSetFloat32UniformCmd
{
	Ren3dShaderVarFloat32Ptr var_;
	float value_;
}; // Ren3dSetFloat32UniformCmd

struct Ren3dSetVec2UniformCmd
{
	Ren3dShaderVarVec2Ptr var_;
	Ren3dVec2 value_;
}; // Ren3dSetVec2UniformCmd

struct Ren3dSetVec4UniformCmd
{
	Ren3dShaderVarVec4Ptr var_;
	Ren3dVec4 value_;
}; // Ren3dSetVec4UniformCmd

struct Ren3dSetMat4UniformCmd
{
	Ren3dShaderVarMat4Ptr var_;
	Ren3dMat4 value_;
}; // Ren3dSetMat4UniformCmd

struct Ren3dSetSampler2dUniformCmd
{
	Ren3dShaderVarSampler2dPtr var_;
	int value_;
}; // Ren3dSetSampler2dUniformCmd

struct Ren3dDrawIndexedCmd
{
	Ren3dDrawIndexedParam draw_indexed_;
}; // Ren3dDrawIndexedCmd


// ==========================================================================
// Ren3dCmdBuffer
//

class Ren3dCmdBuffer
{
protected:
	Ren3dCmdBuffer() = default;

	virtual ~Ren3dCmdBuffer() = default;


public:
	virtual int get_count() const noexcept = 0;


	virtual bool is_enabled() const noexcept = 0;

	virtual void enable(
		const bool is_enabled) = 0;


	virtual void begin_write() = 0;

	virtual void end_write() = 0;

	virtual Ren3dClearCmd* write_clear() = 0;

	virtual Ren3dSetViewportCmd* write_set_viewport() = 0;

	virtual Ren3dEnableScissorCmd* write_enable_scissor() = 0;
	virtual Ren3dSetScissorBoxCmd* write_set_scissor_box() = 0;

	virtual Ren3dEnableCullingCmd* write_enable_culling() = 0;

	virtual Ren3dEnableDepthTestCmd* write_enable_depth_test() = 0;
	virtual Ren3dEnableDepthWriteCmd* write_enable_depth_write() = 0;

	virtual Ren3dEnableBlendingCmd* write_enable_blending() = 0;
	virtual Ren3dSetBlendingFuncCmd* write_set_blending_func() = 0;

	virtual Ren3dSetTextureCmd* write_set_texture() = 0;
	virtual Ren3dSetSamplerCmd* write_set_sampler() = 0;

	virtual Ren3dSetVertexInputCmd* write_set_vertex_input() = 0;

	virtual Ren3dSetShaderStageCmd* write_set_shader_stage() = 0;

	virtual Ren3dSetInt32UniformCmd* write_set_int32_uniform() = 0;
	virtual Ren3dSetFloat32UniformCmd* write_set_float32_uniform() = 0;
	virtual Ren3dSetVec2UniformCmd* write_set_vec2_uniform() = 0;
	virtual Ren3dSetVec4UniformCmd* write_set_vec4_uniform() = 0;
	virtual Ren3dSetMat4UniformCmd* write_set_mat4_uniform() = 0;
	virtual Ren3dSetSampler2dUniformCmd* write_set_sampler_2d_uniform() = 0;

	virtual Ren3dDrawIndexedCmd* write_draw_indexed() = 0;


	virtual void begin_read() = 0;

	virtual void end_read() = 0;

	virtual Ren3dCmdId read_command_id() = 0;

	virtual const Ren3dClearCmd* read_clear() = 0;

	virtual const Ren3dSetViewportCmd* read_set_viewport() = 0;

	virtual const Ren3dEnableScissorCmd* read_enable_scissor() = 0;
	virtual const Ren3dSetScissorBoxCmd* read_set_scissor_box() = 0;

	virtual const Ren3dEnableCullingCmd* read_enable_culling() = 0;

	virtual const Ren3dEnableDepthTestCmd* read_enable_depth_test() = 0;
	virtual const Ren3dEnableDepthWriteCmd* read_enable_depth_write() = 0;

	virtual const Ren3dEnableBlendingCmd* read_enable_blending() = 0;
	virtual const Ren3dSetBlendingFuncCmd* read_set_blending_func() = 0;

	virtual const Ren3dSetTextureCmd* read_set_texture() = 0;
	virtual const Ren3dSetSamplerCmd* read_set_sampler() = 0;

	virtual const Ren3dSetVertexInputCmd* read_set_vertex_input() = 0;

	virtual const Ren3dSetShaderStageCmd* read_set_shader_stage() = 0;

	virtual const Ren3dSetInt32UniformCmd* read_set_int32_uniform() = 0;
	virtual const Ren3dSetFloat32UniformCmd* read_set_float32_uniform() = 0;
	virtual const Ren3dSetVec2UniformCmd* read_set_vec2_uniform() = 0;
	virtual const Ren3dSetVec4UniformCmd* read_set_vec4_uniform() = 0;
	virtual const Ren3dSetMat4UniformCmd* read_set_mat4_uniform() = 0;
	virtual const Ren3dSetSampler2dUniformCmd* read_set_sampler_2d_uniform() = 0;

	virtual const Ren3dDrawIndexedCmd* read_draw_indexed() = 0;
}; // Ren3dCmdBuffer

using Ren3dCmdBufferPtr = Ren3dCmdBuffer*;

//
// Ren3dCmdBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_REN_3D_COMMAND_BUFFER_INCLUDED
