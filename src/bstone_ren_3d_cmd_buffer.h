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


struct Ren3dCmdClear
{
	Ren3dClearParam param_;
}; // Ren3dCmdClear

struct Ren3dCmdViewport
{
	Ren3dViewport viewport_;
}; // Ren3dCmdViewport

struct Ren3dCmdScissor
{
	bool is_enable_;
}; // Ren3dCmdScissor

struct Ren3dCmdScissorBox
{
	Ren3dScissorBox scissor_box_;
}; // Ren3dCmdScissorBox

struct Ren3dCmdCulling
{
	bool is_enable_;
}; // Ren3dCmdCulling

struct Ren3dCmdBlending
{
	bool is_enable_;
}; // Ren3dCmdBlending

struct Ren3dCmdBlendingFunc
{
	Ren3dBlendingFunc blending_func_;
}; // Ren3dCmdBlendingFunc

struct Ren3dCmdDepthTest
{
	bool is_enable_;
}; // Ren3dCmdDepthTest

struct Ren3dCmdDepthWrite
{
	bool is_enable_;
}; // Ren3dCmdDepthWrite

struct Ren3dCmdTexture
{
	Ren3dTexture2dPtr texture_2d_;
}; // Ren3dCmdTexture

struct Ren3dCmdSampler
{
	Ren3dSamplerPtr sampler_;
}; // Ren3dCmdSampler

struct Ren3dCmdVertexInput
{
	Ren3dVertexInputPtr vertex_input_;
}; // Ren3dCmdVertexInput

struct Ren3dCmdShaderStage
{
	Ren3dShaderStagePtr shader_stage_;
}; // Ren3dCmdShaderStage

struct Ren3dCmdShaderVarInt32
{
	Ren3dShaderVarInt32Ptr var_;
	int value_;
}; // Ren3dCmdShaderVarInt32

struct Ren3dCmdShaderVarFloat32
{
	Ren3dShaderVarFloat32Ptr var_;
	float value_;
}; // Ren3dCmdShaderVarFloat32

struct Ren3dCmdShaderVarVec2
{
	Ren3dShaderVarVec2Ptr var_;
	Ren3dVec2 value_;
}; // Ren3dCmdShaderVarVec2

struct Ren3dCmdShaderVarVec4
{
	Ren3dShaderVarVec4Ptr var_;
	Ren3dVec4 value_;
}; // Ren3dCmdShaderVarVec4

struct Ren3dCmdShaderVarMat4
{
	Ren3dShaderVarMat4Ptr var_;
	Ren3dMat4 value_;
}; // Ren3dCmdShaderVarMat4

struct Ren3dCmdShaderVarSampler2d
{
	Ren3dShaderVarSampler2dPtr var_;
	int value_;
}; // Ren3dCmdShaderVarSampler2d

struct Ren3dCmdDrawIndexed
{
	Ren3dDrawIndexedParam param_;
}; // Ren3dCmdDrawIndexed


// ==========================================================================
// Ren3dCmdBuffer
//

class Ren3dCmdBuffer
{
protected:
	Ren3dCmdBuffer() = default;

	virtual ~Ren3dCmdBuffer() = default;


public:
	virtual int get_command_count() const noexcept = 0;


	virtual bool is_enabled() const noexcept = 0;

	virtual void enable(
		const bool is_enabled) = 0;


	virtual void write_begin() = 0;

	virtual void write_end() = 0;

	virtual Ren3dCmdClear* write_clear() = 0;

	virtual Ren3dCmdViewport* write_viewport() = 0;

	virtual Ren3dCmdScissor* write_scissor() = 0;
	virtual Ren3dCmdScissorBox* write_scissor_box() = 0;

	virtual Ren3dCmdCulling* write_culling() = 0;

	virtual Ren3dCmdDepthTest* write_depth_test() = 0;
	virtual Ren3dCmdDepthWrite* write_depth_write() = 0;

	virtual Ren3dCmdBlending* write_blending() = 0;
	virtual Ren3dCmdBlendingFunc* write_blending_func() = 0;

	virtual Ren3dCmdTexture* write_texture() = 0;
	virtual Ren3dCmdSampler* write_sampler() = 0;

	virtual Ren3dCmdVertexInput* write_vertex_input() = 0;

	virtual Ren3dCmdShaderStage* write_shader_stage() = 0;

	virtual Ren3dCmdShaderVarInt32* write_shader_var_int32() = 0;
	virtual Ren3dCmdShaderVarFloat32* write_shader_var_float32() = 0;
	virtual Ren3dCmdShaderVarVec2* write_shader_var_vec2() = 0;
	virtual Ren3dCmdShaderVarVec4* write_shader_var_vec4() = 0;
	virtual Ren3dCmdShaderVarMat4* write_shader_var_mat4() = 0;
	virtual Ren3dCmdShaderVarSampler2d* write_shader_var_sampler_2d() = 0;

	virtual Ren3dCmdDrawIndexed* write_draw_indexed() = 0;


	virtual void read_begin() = 0;

	virtual void read_end() = 0;

	virtual Ren3dCmdId read_command_id() = 0;

	virtual const Ren3dCmdClear* read_clear() = 0;

	virtual const Ren3dCmdViewport* read_viewport() = 0;

	virtual const Ren3dCmdScissor* read_scissor() = 0;
	virtual const Ren3dCmdScissorBox* read_scissor_box() = 0;

	virtual const Ren3dCmdCulling* read_culling() = 0;

	virtual const Ren3dCmdDepthTest* read_depth_test() = 0;
	virtual const Ren3dCmdDepthWrite* read_depth_write() = 0;

	virtual const Ren3dCmdBlending* read_blending() = 0;
	virtual const Ren3dCmdBlendingFunc* read_blending_func() = 0;

	virtual const Ren3dCmdTexture* read_texture() = 0;
	virtual const Ren3dCmdSampler* read_sampler() = 0;

	virtual const Ren3dCmdVertexInput* read_vertex_input() = 0;

	virtual const Ren3dCmdShaderStage* read_shader_stage() = 0;

	virtual const Ren3dCmdShaderVarInt32* read_shader_var_int32() = 0;
	virtual const Ren3dCmdShaderVarFloat32* read_shader_var_float32() = 0;
	virtual const Ren3dCmdShaderVarVec2* read_shader_var_vec2() = 0;
	virtual const Ren3dCmdShaderVarVec4* read_shader_var_vec4() = 0;
	virtual const Ren3dCmdShaderVarMat4* read_shader_var_mat4() = 0;
	virtual const Ren3dCmdShaderVarSampler2d* read_shader_var_sampler_2d() = 0;

	virtual const Ren3dCmdDrawIndexed* read_draw_indexed() = 0;
}; // Ren3dCmdBuffer

using Ren3dCmdBufferPtr = Ren3dCmdBuffer*;

//
// Ren3dCmdBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_REN_3D_COMMAND_BUFFER_INCLUDED
