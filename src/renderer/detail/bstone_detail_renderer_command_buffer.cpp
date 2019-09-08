/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

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
// Renderer's command buffer (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_renderer_command_buffer.h"

#include <algorithm>


namespace bstone
{
namespace detail
{


// ==========================================================================
// RendererCommandBuffer
//

RendererCommandBuffer::RendererCommandBuffer()
	:
	is_enabled_{},
	is_reading_{},
	is_writing_{},
	size_{},
	write_offset_{},
	read_offset_{},
	resize_delta_size_{},
	command_count_{},
	data_{}
{
}

RendererCommandBuffer::RendererCommandBuffer(
	RendererCommandBuffer&& rhs)
	:
	is_enabled_{std::move(rhs.is_enabled_)},
	is_reading_{std::move(rhs.is_reading_)},
	is_writing_{std::move(rhs.is_writing_)},
	size_{std::move(rhs.size_)},
	write_offset_{std::move(rhs.write_offset_)},
	read_offset_{std::move(rhs.read_offset_)},
	resize_delta_size_{std::move(rhs.resize_delta_size_)},
	command_count_{std::move(rhs.command_count_)},
	data_{std::move(rhs.data_)}
{
}

RendererCommandBuffer::~RendererCommandBuffer()
{
}

int RendererCommandBuffer::get_command_count() const noexcept
{
	return command_count_;
}

bool RendererCommandBuffer::is_enabled() const noexcept
{
	return is_enabled_;
}

void RendererCommandBuffer::enable(
	const bool is_enabled)
{
	is_enabled_ = is_enabled;
}

void RendererCommandBuffer::write_begin()
{
	if (is_reading_ || is_writing_)
	{
		assert(!"Invalid state.");

		return;
	}

	is_writing_ = true;
	write_offset_ = 0;
	command_count_ = 0;
}

void RendererCommandBuffer::write_end()
{
	if (is_reading_ || !is_writing_)
	{
		assert(!"Invalid state.");

		return;
	}

	is_writing_ = false;
}

RendererCommandViewport* RendererCommandBuffer::write_viewport()
{
	return write<RendererCommandViewport>(RendererCommandId::viewport);
}

RendererCommandScissor* RendererCommandBuffer::write_scissor()
{
	return write<RendererCommandScissor>(RendererCommandId::scissor);
}

RendererCommandScissorBox* RendererCommandBuffer::write_scissor_box()
{
	return write<RendererCommandScissorBox>(RendererCommandId::scissor_set_box);
}

RendererCommandCulling* RendererCommandBuffer::write_culling()
{
	return write<RendererCommandCulling>(RendererCommandId::culling);
}

RendererCommandDepthTest* RendererCommandBuffer::write_depth_test()
{
	return write<RendererCommandDepthTest>(RendererCommandId::depth_set_test);
}

RendererCommandDepthWrite* RendererCommandBuffer::write_depth_write()
{
	return write<RendererCommandDepthWrite>(RendererCommandId::depth_set_write);
}

RendererCommandBlending* RendererCommandBuffer::write_blending()
{
	return write<RendererCommandBlending>(RendererCommandId::blending);
}

RendererCommandBlendingFunc* RendererCommandBuffer::write_blending_func()
{
	return write<RendererCommandBlendingFunc>(RendererCommandId::blending_func);
}

RendererCommandTexture* RendererCommandBuffer::write_texture()
{
	return write<RendererCommandTexture>(RendererCommandId::texture);
}

RendererCommandSampler* RendererCommandBuffer::write_sampler()
{
	return write<RendererCommandSampler>(RendererCommandId::sampler);
}

RendererCommandVertexInput* RendererCommandBuffer::write_vertex_input()
{
	return write<RendererCommandVertexInput>(RendererCommandId::vertex_input);
}

RendererCommandShaderStage* RendererCommandBuffer::write_shader_stage()
{
	return write<RendererCommandShaderStage>(RendererCommandId::shader_stage);
}

RendererCommandShaderVarInt32* RendererCommandBuffer::write_shader_var_int32()
{
	return write<RendererCommandShaderVarInt32>(RendererCommandId::shader_var_int32);
}

RendererCommandShaderVarFloat32* RendererCommandBuffer::write_shader_var_float32()
{
	return write<RendererCommandShaderVarFloat32>(RendererCommandId::shader_var_float32);
}

RendererCommandShaderVarVec2* RendererCommandBuffer::write_shader_var_vec2()
{
	return write<RendererCommandShaderVarVec2>(RendererCommandId::shader_var_vec2);
}

RendererCommandShaderVarVec4* RendererCommandBuffer::write_shader_var_vec4()
{
	return write<RendererCommandShaderVarVec4>(RendererCommandId::shader_var_vec4);
}

RendererCommandShaderVarMat4* RendererCommandBuffer::write_shader_var_mat4()
{
	return write<RendererCommandShaderVarMat4>(RendererCommandId::shader_var_mat4);
}

RendererCommandShaderVarSampler2d* RendererCommandBuffer::write_shader_var_sampler_2d()
{
	return write<RendererCommandShaderVarSampler2d>(RendererCommandId::shader_var_sampler2d);
}

RendererCommandDrawQuads* RendererCommandBuffer::write_draw_quads()
{
	return write<RendererCommandDrawQuads>(RendererCommandId::draw_quads);
}

void RendererCommandBuffer::read_begin()
{
	if (is_reading_ || is_writing_)
	{
		assert(!"Invalid state.");

		return;
	}

	is_reading_ = true;
	read_offset_ = 0;
}

void RendererCommandBuffer::read_end()
{
	if (!is_reading_ || is_writing_)
	{
		assert(!"Invalid state.");

		return;
	}

	assert(write_offset_ == read_offset_);

	is_reading_ = false;
}

RendererCommandId RendererCommandBuffer::read_command_id()
{
	const auto command_id = read<RendererCommandId>();

	if (!command_id)
	{
		return RendererCommandId::none;
	}

	return *command_id;
}

const RendererCommandViewport* RendererCommandBuffer::read_viewport()
{
	return read<RendererCommandViewport>();
}

const RendererCommandScissor* RendererCommandBuffer::read_scissor()
{
	return read<RendererCommandScissor>();
}

const RendererCommandScissorBox* RendererCommandBuffer::read_scissor_box()
{
	return read<RendererCommandScissorBox>();
}

const RendererCommandCulling* RendererCommandBuffer::read_culling()
{
	return read<RendererCommandCulling>();
}

const RendererCommandDepthTest* RendererCommandBuffer::read_depth_test()
{
	return read<RendererCommandDepthTest>();
}

const RendererCommandDepthWrite* RendererCommandBuffer::read_depth_write()
{
	return read<RendererCommandDepthWrite>();
}

const RendererCommandBlending* RendererCommandBuffer::read_blending()
{
	return read<RendererCommandBlending>();
}

const RendererCommandBlendingFunc* RendererCommandBuffer::read_blending_func()
{
	return read<RendererCommandBlendingFunc>();
}

const RendererCommandTexture* RendererCommandBuffer::read_texture()
{
	return read<RendererCommandTexture>();
}

const RendererCommandSampler* RendererCommandBuffer::read_sampler()
{
	return read<RendererCommandSampler>();
}

const RendererCommandVertexInput* RendererCommandBuffer::read_vertex_input()
{
	return read<RendererCommandVertexInput>();
}

const RendererCommandShaderStage* RendererCommandBuffer::read_shader_stage()
{
	return read<RendererCommandShaderStage>();
}

const RendererCommandShaderVarInt32* RendererCommandBuffer::read_shader_var_int32()
{
	return read<RendererCommandShaderVarInt32>();
}

const RendererCommandShaderVarFloat32* RendererCommandBuffer::read_shader_var_float32()
{
	return read<RendererCommandShaderVarFloat32>();
}

const RendererCommandShaderVarVec2* RendererCommandBuffer::read_shader_var_vec2()
{
	return read<RendererCommandShaderVarVec2>();
}

const RendererCommandShaderVarVec4* RendererCommandBuffer::read_shader_var_vec4()
{
	return read<RendererCommandShaderVarVec4>();
}

const RendererCommandShaderVarMat4* RendererCommandBuffer::read_shader_var_mat4()
{
	return read<RendererCommandShaderVarMat4>();
}

const RendererCommandShaderVarSampler2d* RendererCommandBuffer::read_shader_var_sampler_2d()
{
	return read<RendererCommandShaderVarSampler2d>();
}

const RendererCommandDrawQuads* RendererCommandBuffer::read_draw_quads()
{
	return read<RendererCommandDrawQuads>();
}

void RendererCommandBuffer::initialize(
	const RendererCommandManagerBufferAddParam& param)
{
	is_reading_ = false;
	is_writing_ = false;

	size_ = std::max(param.initial_size_, get_min_initial_size());
	write_offset_ = 0;
	read_offset_ = 0;
	resize_delta_size_ = std::max(param.resize_delta_size_, get_min_resize_delta_size());

	data_.resize(size_);
}

void RendererCommandBuffer::resize_if_necessary(
	const int dst_delta_size)
{
	assert(dst_delta_size > 0);

	if ((size_ - write_offset_) >= dst_delta_size)
	{
		return;
	}

	size_ += resize_delta_size_;

	data_.resize(size_);
}

//
// RendererCommandBuffer
// ==========================================================================


} // detail
} // bstone