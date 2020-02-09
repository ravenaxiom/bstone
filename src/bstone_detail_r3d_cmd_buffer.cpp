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
// 3D renderer's command buffer (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_cmd_buffer.h"

#include <algorithm>

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandBufferImplInitException
//

class Renderer3dCommandBufferImplInitException :
	public Exception
{
public:
	explicit Renderer3dCommandBufferImplInitException(
		const char* const message)
		:
		Exception{std::string{"[R3D_CMD_BUF_INIT] "} + message}
	{
	}
}; // Renderer3dCommandBufferImplInitException

//
// Renderer3dCommandBufferImplInitException
// ==========================================================================


// ==========================================================================
// Renderer3dCommandBufferImpl
//

Renderer3dCommandBufferImpl::Renderer3dCommandBufferImpl(
	const Renderer3dCommandQueueEnqueueParam& param)
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
	validate_param(param);

	size_ = std::max(param.initial_size_, get_min_initial_size());
	resize_delta_size_ = std::max(param.resize_delta_size_, get_min_resize_delta_size());

	data_.resize(size_);
}

Renderer3dCommandBufferImpl::~Renderer3dCommandBufferImpl() = default;

int Renderer3dCommandBufferImpl::get_command_count() const noexcept
{
	return command_count_;
}

bool Renderer3dCommandBufferImpl::is_enabled() const noexcept
{
	return is_enabled_;
}

void Renderer3dCommandBufferImpl::enable(
	const bool is_enabled)
{
	is_enabled_ = is_enabled;
}

void Renderer3dCommandBufferImpl::write_begin()
{
	if (is_reading_)
	{
		throw Exception{"Already reading."};
	}

	if (is_writing_)
	{
		throw Exception{"Already writing."};
	}

	is_writing_ = true;
	write_offset_ = 0;
	command_count_ = 0;
}

void Renderer3dCommandBufferImpl::write_end()
{
	if (is_reading_)
	{
		throw Exception{"Already reading."};
	}

	if (!is_writing_)
	{
		throw Exception{"Not writing."};
	}

	is_writing_ = false;
}

Renderer3dCommandClear* Renderer3dCommandBufferImpl::write_clear()
{
	return write<Renderer3dCommandClear>(Renderer3dCommandId::clear);
}

Renderer3dCommandViewport* Renderer3dCommandBufferImpl::write_viewport()
{
	return write<Renderer3dCommandViewport>(Renderer3dCommandId::viewport);
}

Renderer3dCommandScissor* Renderer3dCommandBufferImpl::write_scissor()
{
	return write<Renderer3dCommandScissor>(Renderer3dCommandId::scissor);
}

Renderer3dCommandScissorBox* Renderer3dCommandBufferImpl::write_scissor_box()
{
	return write<Renderer3dCommandScissorBox>(Renderer3dCommandId::scissor_set_box);
}

Renderer3dCommandCulling* Renderer3dCommandBufferImpl::write_culling()
{
	return write<Renderer3dCommandCulling>(Renderer3dCommandId::culling);
}

Renderer3dCommandDepthTest* Renderer3dCommandBufferImpl::write_depth_test()
{
	return write<Renderer3dCommandDepthTest>(Renderer3dCommandId::depth_set_test);
}

Renderer3dCommandDepthWrite* Renderer3dCommandBufferImpl::write_depth_write()
{
	return write<Renderer3dCommandDepthWrite>(Renderer3dCommandId::depth_set_write);
}

Renderer3dCommandBlending* Renderer3dCommandBufferImpl::write_blending()
{
	return write<Renderer3dCommandBlending>(Renderer3dCommandId::blending);
}

Renderer3dCommandBlendingFunc* Renderer3dCommandBufferImpl::write_blending_func()
{
	return write<Renderer3dCommandBlendingFunc>(Renderer3dCommandId::blending_func);
}

Renderer3dCommandTexture* Renderer3dCommandBufferImpl::write_texture()
{
	return write<Renderer3dCommandTexture>(Renderer3dCommandId::texture);
}

Renderer3dCommandSampler* Renderer3dCommandBufferImpl::write_sampler()
{
	return write<Renderer3dCommandSampler>(Renderer3dCommandId::sampler);
}

Renderer3dCommandVertexInput* Renderer3dCommandBufferImpl::write_vertex_input()
{
	return write<Renderer3dCommandVertexInput>(Renderer3dCommandId::vertex_input);
}

Renderer3dCommandShaderStage* Renderer3dCommandBufferImpl::write_shader_stage()
{
	return write<Renderer3dCommandShaderStage>(Renderer3dCommandId::shader_stage);
}

Renderer3dCommandShaderVarInt32* Renderer3dCommandBufferImpl::write_shader_var_int32()
{
	return write<Renderer3dCommandShaderVarInt32>(Renderer3dCommandId::shader_var_int32);
}

Renderer3dCommandShaderVarFloat32* Renderer3dCommandBufferImpl::write_shader_var_float32()
{
	return write<Renderer3dCommandShaderVarFloat32>(Renderer3dCommandId::shader_var_float32);
}

Renderer3dCommandShaderVarVec2* Renderer3dCommandBufferImpl::write_shader_var_vec2()
{
	return write<Renderer3dCommandShaderVarVec2>(Renderer3dCommandId::shader_var_vec2);
}

Renderer3dCommandShaderVarVec4* Renderer3dCommandBufferImpl::write_shader_var_vec4()
{
	return write<Renderer3dCommandShaderVarVec4>(Renderer3dCommandId::shader_var_vec4);
}

Renderer3dCommandShaderVarMat4* Renderer3dCommandBufferImpl::write_shader_var_mat4()
{
	return write<Renderer3dCommandShaderVarMat4>(Renderer3dCommandId::shader_var_mat4);
}

Renderer3dCommandShaderVarSampler2d* Renderer3dCommandBufferImpl::write_shader_var_sampler_2d()
{
	return write<Renderer3dCommandShaderVarSampler2d>(Renderer3dCommandId::shader_var_sampler_2d);
}

Renderer3dCommandDrawIndexed* Renderer3dCommandBufferImpl::write_draw_indexed()
{
	return write<Renderer3dCommandDrawIndexed>(Renderer3dCommandId::draw_indexed);
}

void Renderer3dCommandBufferImpl::read_begin()
{
	if (is_reading_)
	{
		throw Exception{"Already reading."};
	}

	if (is_writing_)
	{
		throw Exception{"Already writing."};
	}

	is_reading_ = true;
	read_offset_ = 0;
}

void Renderer3dCommandBufferImpl::read_end()
{
	if (!is_reading_)
	{
		throw Exception{"Not reading."};
	}

	if (is_writing_)
	{
		throw Exception{"Already writing."};
	}

	if (write_offset_ != read_offset_)
	{
		throw Exception{"Offset mismatch."};
	}

	is_reading_ = false;
}

Renderer3dCommandId Renderer3dCommandBufferImpl::read_command_id()
{
	const auto command_id = read<Renderer3dCommandId>();

	if (!command_id)
	{
		return Renderer3dCommandId::none;
	}

	return *command_id;
}

const Renderer3dCommandClear* Renderer3dCommandBufferImpl::read_clear()
{
	return read<Renderer3dCommandClear>();
}

const Renderer3dCommandViewport* Renderer3dCommandBufferImpl::read_viewport()
{
	return read<Renderer3dCommandViewport>();
}

const Renderer3dCommandScissor* Renderer3dCommandBufferImpl::read_scissor()
{
	return read<Renderer3dCommandScissor>();
}

const Renderer3dCommandScissorBox* Renderer3dCommandBufferImpl::read_scissor_box()
{
	return read<Renderer3dCommandScissorBox>();
}

const Renderer3dCommandCulling* Renderer3dCommandBufferImpl::read_culling()
{
	return read<Renderer3dCommandCulling>();
}

const Renderer3dCommandDepthTest* Renderer3dCommandBufferImpl::read_depth_test()
{
	return read<Renderer3dCommandDepthTest>();
}

const Renderer3dCommandDepthWrite* Renderer3dCommandBufferImpl::read_depth_write()
{
	return read<Renderer3dCommandDepthWrite>();
}

const Renderer3dCommandBlending* Renderer3dCommandBufferImpl::read_blending()
{
	return read<Renderer3dCommandBlending>();
}

const Renderer3dCommandBlendingFunc* Renderer3dCommandBufferImpl::read_blending_func()
{
	return read<Renderer3dCommandBlendingFunc>();
}

const Renderer3dCommandTexture* Renderer3dCommandBufferImpl::read_texture()
{
	return read<Renderer3dCommandTexture>();
}

const Renderer3dCommandSampler* Renderer3dCommandBufferImpl::read_sampler()
{
	return read<Renderer3dCommandSampler>();
}

const Renderer3dCommandVertexInput* Renderer3dCommandBufferImpl::read_vertex_input()
{
	return read<Renderer3dCommandVertexInput>();
}

const Renderer3dCommandShaderStage* Renderer3dCommandBufferImpl::read_shader_stage()
{
	return read<Renderer3dCommandShaderStage>();
}

const Renderer3dCommandShaderVarInt32* Renderer3dCommandBufferImpl::read_shader_var_int32()
{
	return read<Renderer3dCommandShaderVarInt32>();
}

const Renderer3dCommandShaderVarFloat32* Renderer3dCommandBufferImpl::read_shader_var_float32()
{
	return read<Renderer3dCommandShaderVarFloat32>();
}

const Renderer3dCommandShaderVarVec2* Renderer3dCommandBufferImpl::read_shader_var_vec2()
{
	return read<Renderer3dCommandShaderVarVec2>();
}

const Renderer3dCommandShaderVarVec4* Renderer3dCommandBufferImpl::read_shader_var_vec4()
{
	return read<Renderer3dCommandShaderVarVec4>();
}

const Renderer3dCommandShaderVarMat4* Renderer3dCommandBufferImpl::read_shader_var_mat4()
{
	return read<Renderer3dCommandShaderVarMat4>();
}

const Renderer3dCommandShaderVarSampler2d* Renderer3dCommandBufferImpl::read_shader_var_sampler_2d()
{
	return read<Renderer3dCommandShaderVarSampler2d>();
}

const Renderer3dCommandDrawIndexed* Renderer3dCommandBufferImpl::read_draw_indexed()
{
	return read<Renderer3dCommandDrawIndexed>();
}

void Renderer3dCommandBufferImpl::validate_param(
	const Renderer3dCommandQueueEnqueueParam& param)
{
	if (param.initial_size_ <= 0)
	{
		throw Renderer3dCommandBufferImplInitException{"Initial size out of range."};
	}

	if (param.resize_delta_size_ < 0)
	{
		throw Renderer3dCommandBufferImplInitException{"Resize delta out of range."};
	}
}

void Renderer3dCommandBufferImpl::resize_if_necessary(
	const int dst_delta_size)
{
	if (dst_delta_size <= 0)
	{
		throw Exception{"Resize delta out of range."};
	}

	if ((size_ - write_offset_) >= dst_delta_size)
	{
		return;
	}

	size_ += resize_delta_size_;

	data_.resize(size_);
}

//
// Renderer3dCommandBufferImpl
// ==========================================================================


} // detail
} // bstone
