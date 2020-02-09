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


#ifndef BSTONE_R3D_COMMAND_BUFFER_INCLUDED
#define BSTONE_R3D_COMMAND_BUFFER_INCLUDED


#include "bstone_r3d.h"


namespace bstone
{


enum class Renderer3dCommandId
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
}; // Renderer3dCommandId


struct Renderer3dCommandClear
{
	Renderer3dClearParam param_;
}; // Renderer3dCommandClear

struct Renderer3dCommandViewport
{
	Renderer3dViewport viewport_;
}; // Renderer3dCommandViewport

struct Renderer3dCommandScissor
{
	bool is_enable_;
}; // Renderer3dCommandScissor

struct Renderer3dCommandScissorBox
{
	Renderer3dScissorBox scissor_box_;
}; // Renderer3dCommandScissorBox

struct Renderer3dCommandCulling
{
	bool is_enable_;
}; // Renderer3dCommandCulling

struct Renderer3dCommandBlending
{
	bool is_enable_;
}; // Renderer3dCommandBlending

struct Renderer3dCommandBlendingFunc
{
	Renderer3dBlendingFunc blending_func_;
}; // Renderer3dCommandBlendingFunc

struct Renderer3dCommandDepthTest
{
	bool is_enable_;
}; // Renderer3dCommandDepthTest

struct Renderer3dCommandDepthWrite
{
	bool is_enable_;
}; // Renderer3dCommandDepthWrite

struct Renderer3dCommandTexture
{
	Renderer3dTexture2dPtr texture_2d_;
}; // Renderer3dCommandTexture

struct Renderer3dCommandSampler
{
	Renderer3dSamplerPtr sampler_;
}; // Renderer3dCommandSampler

struct Renderer3dCommandVertexInput
{
	Renderer3dVertexInputPtr vertex_input_;
}; // Renderer3dCommandVertexInput

struct Renderer3dCommandShaderStage
{
	Renderer3dShaderStagePtr shader_stage_;
}; // Renderer3dCommandShaderStage

struct Renderer3dCommandShaderVarInt32
{
	Renderer3dShaderVarInt32Ptr var_;
	int value_;
}; // Renderer3dCommandShaderVarInt32

struct Renderer3dCommandShaderVarFloat32
{
	Renderer3dShaderVarFloat32Ptr var_;
	float value_;
}; // Renderer3dCommandShaderVarFloat32

struct Renderer3dCommandShaderVarVec2
{
	Renderer3dShaderVarVec2Ptr var_;
	Renderer3dVec2 value_;
}; // Renderer3dCommandShaderVarVec2

struct Renderer3dCommandShaderVarVec4
{
	Renderer3dShaderVarVec4Ptr var_;
	Renderer3dVec4 value_;
}; // Renderer3dCommandShaderVarVec4

struct Renderer3dCommandShaderVarMat4
{
	Renderer3dShaderVarMat4Ptr var_;
	Renderer3dMat4 value_;
}; // Renderer3dCommandShaderVarMat4

struct Renderer3dCommandShaderVarSampler2d
{
	Renderer3dShaderVarSampler2dPtr var_;
	int value_;
}; // Renderer3dCommandShaderVarSampler2d

struct Renderer3dCommandDrawIndexed
{
	Renderer3dDrawIndexedParam param_;
}; // Renderer3dCommandDrawIndexed


// ==========================================================================
// Renderer3dCommandBuffer
//

class Renderer3dCommandBuffer
{
protected:
	Renderer3dCommandBuffer() = default;

	virtual ~Renderer3dCommandBuffer() = default;


public:
	virtual int get_command_count() const noexcept = 0;


	virtual bool is_enabled() const noexcept = 0;

	virtual void enable(
		const bool is_enabled) = 0;


	virtual void write_begin() = 0;

	virtual void write_end() = 0;

	virtual Renderer3dCommandClear* write_clear() = 0;

	virtual Renderer3dCommandViewport* write_viewport() = 0;

	virtual Renderer3dCommandScissor* write_scissor() = 0;
	virtual Renderer3dCommandScissorBox* write_scissor_box() = 0;

	virtual Renderer3dCommandCulling* write_culling() = 0;

	virtual Renderer3dCommandDepthTest* write_depth_test() = 0;
	virtual Renderer3dCommandDepthWrite* write_depth_write() = 0;

	virtual Renderer3dCommandBlending* write_blending() = 0;
	virtual Renderer3dCommandBlendingFunc* write_blending_func() = 0;

	virtual Renderer3dCommandTexture* write_texture() = 0;
	virtual Renderer3dCommandSampler* write_sampler() = 0;

	virtual Renderer3dCommandVertexInput* write_vertex_input() = 0;

	virtual Renderer3dCommandShaderStage* write_shader_stage() = 0;

	virtual Renderer3dCommandShaderVarInt32* write_shader_var_int32() = 0;
	virtual Renderer3dCommandShaderVarFloat32* write_shader_var_float32() = 0;
	virtual Renderer3dCommandShaderVarVec2* write_shader_var_vec2() = 0;
	virtual Renderer3dCommandShaderVarVec4* write_shader_var_vec4() = 0;
	virtual Renderer3dCommandShaderVarMat4* write_shader_var_mat4() = 0;
	virtual Renderer3dCommandShaderVarSampler2d* write_shader_var_sampler_2d() = 0;

	virtual Renderer3dCommandDrawIndexed* write_draw_indexed() = 0;


	virtual void read_begin() = 0;

	virtual void read_end() = 0;

	virtual Renderer3dCommandId read_command_id() = 0;

	virtual const Renderer3dCommandClear* read_clear() = 0;

	virtual const Renderer3dCommandViewport* read_viewport() = 0;

	virtual const Renderer3dCommandScissor* read_scissor() = 0;
	virtual const Renderer3dCommandScissorBox* read_scissor_box() = 0;

	virtual const Renderer3dCommandCulling* read_culling() = 0;

	virtual const Renderer3dCommandDepthTest* read_depth_test() = 0;
	virtual const Renderer3dCommandDepthWrite* read_depth_write() = 0;

	virtual const Renderer3dCommandBlending* read_blending() = 0;
	virtual const Renderer3dCommandBlendingFunc* read_blending_func() = 0;

	virtual const Renderer3dCommandTexture* read_texture() = 0;
	virtual const Renderer3dCommandSampler* read_sampler() = 0;

	virtual const Renderer3dCommandVertexInput* read_vertex_input() = 0;

	virtual const Renderer3dCommandShaderStage* read_shader_stage() = 0;

	virtual const Renderer3dCommandShaderVarInt32* read_shader_var_int32() = 0;
	virtual const Renderer3dCommandShaderVarFloat32* read_shader_var_float32() = 0;
	virtual const Renderer3dCommandShaderVarVec2* read_shader_var_vec2() = 0;
	virtual const Renderer3dCommandShaderVarVec4* read_shader_var_vec4() = 0;
	virtual const Renderer3dCommandShaderVarMat4* read_shader_var_mat4() = 0;
	virtual const Renderer3dCommandShaderVarSampler2d* read_shader_var_sampler_2d() = 0;

	virtual const Renderer3dCommandDrawIndexed* read_draw_indexed() = 0;
}; // Renderer3dCommandBuffer

using Renderer3dCommandBufferPtr = Renderer3dCommandBuffer*;

//
// Renderer3dCommandBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_COMMAND_BUFFER_INCLUDED
