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


enum class R3dCmdId
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
}; // R3dCmdId


struct R3dCmdClear
{
	R3dClearParam param_;
}; // R3dCmdClear

struct R3dCmdViewport
{
	R3dViewport viewport_;
}; // R3dCmdViewport

struct R3dCmdScissor
{
	bool is_enable_;
}; // R3dCmdScissor

struct R3dCmdScissorBox
{
	R3dScissorBox scissor_box_;
}; // R3dCmdScissorBox

struct R3dCmdCulling
{
	bool is_enable_;
}; // R3dCmdCulling

struct R3dCmdBlending
{
	bool is_enable_;
}; // R3dCmdBlending

struct R3dCmdBlendingFunc
{
	R3dBlendingFunc blending_func_;
}; // R3dCmdBlendingFunc

struct R3dCmdDepthTest
{
	bool is_enable_;
}; // R3dCmdDepthTest

struct R3dCmdDepthWrite
{
	bool is_enable_;
}; // R3dCmdDepthWrite

struct R3dCmdTexture
{
	R3dTexture2dPtr texture_2d_;
}; // R3dCmdTexture

struct R3dCmdSampler
{
	R3dSamplerPtr sampler_;
}; // R3dCmdSampler

struct R3dCmdVertexInput
{
	R3dVertexInputPtr vertex_input_;
}; // R3dCmdVertexInput

struct R3dCmdShaderStage
{
	R3dShaderStagePtr shader_stage_;
}; // R3dCmdShaderStage

struct R3dCmdShaderVarInt32
{
	R3dShaderVarInt32Ptr var_;
	int value_;
}; // R3dCmdShaderVarInt32

struct R3dCmdShaderVarFloat32
{
	R3dShaderVarFloat32Ptr var_;
	float value_;
}; // R3dCmdShaderVarFloat32

struct R3dCmdShaderVarVec2
{
	R3dShaderVarVec2Ptr var_;
	R3dVec2 value_;
}; // R3dCmdShaderVarVec2

struct R3dCmdShaderVarVec4
{
	R3dShaderVarVec4Ptr var_;
	R3dVec4 value_;
}; // R3dCmdShaderVarVec4

struct R3dCmdShaderVarMat4
{
	R3dShaderVarMat4Ptr var_;
	R3dMat4 value_;
}; // R3dCmdShaderVarMat4

struct R3dCmdShaderVarSampler2d
{
	R3dShaderVarSampler2dPtr var_;
	int value_;
}; // R3dCmdShaderVarSampler2d

struct R3dCmdDrawIndexed
{
	R3dDrawIndexedParam param_;
}; // R3dCmdDrawIndexed


// ==========================================================================
// R3dCmdBuffer
//

class R3dCmdBuffer
{
protected:
	R3dCmdBuffer() = default;

	virtual ~R3dCmdBuffer() = default;


public:
	virtual int get_command_count() const noexcept = 0;


	virtual bool is_enabled() const noexcept = 0;

	virtual void enable(
		const bool is_enabled) = 0;


	virtual void write_begin() = 0;

	virtual void write_end() = 0;

	virtual R3dCmdClear* write_clear() = 0;

	virtual R3dCmdViewport* write_viewport() = 0;

	virtual R3dCmdScissor* write_scissor() = 0;
	virtual R3dCmdScissorBox* write_scissor_box() = 0;

	virtual R3dCmdCulling* write_culling() = 0;

	virtual R3dCmdDepthTest* write_depth_test() = 0;
	virtual R3dCmdDepthWrite* write_depth_write() = 0;

	virtual R3dCmdBlending* write_blending() = 0;
	virtual R3dCmdBlendingFunc* write_blending_func() = 0;

	virtual R3dCmdTexture* write_texture() = 0;
	virtual R3dCmdSampler* write_sampler() = 0;

	virtual R3dCmdVertexInput* write_vertex_input() = 0;

	virtual R3dCmdShaderStage* write_shader_stage() = 0;

	virtual R3dCmdShaderVarInt32* write_shader_var_int32() = 0;
	virtual R3dCmdShaderVarFloat32* write_shader_var_float32() = 0;
	virtual R3dCmdShaderVarVec2* write_shader_var_vec2() = 0;
	virtual R3dCmdShaderVarVec4* write_shader_var_vec4() = 0;
	virtual R3dCmdShaderVarMat4* write_shader_var_mat4() = 0;
	virtual R3dCmdShaderVarSampler2d* write_shader_var_sampler_2d() = 0;

	virtual R3dCmdDrawIndexed* write_draw_indexed() = 0;


	virtual void read_begin() = 0;

	virtual void read_end() = 0;

	virtual R3dCmdId read_command_id() = 0;

	virtual const R3dCmdClear* read_clear() = 0;

	virtual const R3dCmdViewport* read_viewport() = 0;

	virtual const R3dCmdScissor* read_scissor() = 0;
	virtual const R3dCmdScissorBox* read_scissor_box() = 0;

	virtual const R3dCmdCulling* read_culling() = 0;

	virtual const R3dCmdDepthTest* read_depth_test() = 0;
	virtual const R3dCmdDepthWrite* read_depth_write() = 0;

	virtual const R3dCmdBlending* read_blending() = 0;
	virtual const R3dCmdBlendingFunc* read_blending_func() = 0;

	virtual const R3dCmdTexture* read_texture() = 0;
	virtual const R3dCmdSampler* read_sampler() = 0;

	virtual const R3dCmdVertexInput* read_vertex_input() = 0;

	virtual const R3dCmdShaderStage* read_shader_stage() = 0;

	virtual const R3dCmdShaderVarInt32* read_shader_var_int32() = 0;
	virtual const R3dCmdShaderVarFloat32* read_shader_var_float32() = 0;
	virtual const R3dCmdShaderVarVec2* read_shader_var_vec2() = 0;
	virtual const R3dCmdShaderVarVec4* read_shader_var_vec4() = 0;
	virtual const R3dCmdShaderVarMat4* read_shader_var_mat4() = 0;
	virtual const R3dCmdShaderVarSampler2d* read_shader_var_sampler_2d() = 0;

	virtual const R3dCmdDrawIndexed* read_draw_indexed() = 0;
}; // R3dCmdBuffer

using R3dCmdBufferPtr = R3dCmdBuffer*;

//
// R3dCmdBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_COMMAND_BUFFER_INCLUDED
