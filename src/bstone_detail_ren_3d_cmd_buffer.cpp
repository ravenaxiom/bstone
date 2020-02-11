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
#include "bstone_detail_ren_3d_cmd_buffer.h"

#include <algorithm>

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dCmdBufferImplInitException
//

class Ren3dCmdBufferImplInitException :
	public Exception
{
public:
	explicit Ren3dCmdBufferImplInitException(
		const char* const message)
		:
		Exception{std::string{"[R3D_CMD_BUF_INIT] "} + message}
	{
	}
}; // Ren3dCmdBufferImplInitException

//
// Ren3dCmdBufferImplInitException
// ==========================================================================


// ==========================================================================
// Ren3dCmdBufferImpl
//

Ren3dCmdBufferImpl::Ren3dCmdBufferImpl(
	const Ren3dCmdQueueEnqueueParam& param)
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

Ren3dCmdBufferImpl::~Ren3dCmdBufferImpl() = default;

int Ren3dCmdBufferImpl::get_command_count() const noexcept
{
	return command_count_;
}

bool Ren3dCmdBufferImpl::is_enabled() const noexcept
{
	return is_enabled_;
}

void Ren3dCmdBufferImpl::enable(
	const bool is_enabled)
{
	is_enabled_ = is_enabled;
}

void Ren3dCmdBufferImpl::write_begin()
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

void Ren3dCmdBufferImpl::write_end()
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

Ren3dCmdClear* Ren3dCmdBufferImpl::write_clear()
{
	return write<Ren3dCmdClear>(Ren3dCmdId::clear);
}

Ren3dCmdViewport* Ren3dCmdBufferImpl::write_viewport()
{
	return write<Ren3dCmdViewport>(Ren3dCmdId::viewport);
}

Ren3dCmdScissor* Ren3dCmdBufferImpl::write_scissor()
{
	return write<Ren3dCmdScissor>(Ren3dCmdId::scissor);
}

Ren3dCmdScissorBox* Ren3dCmdBufferImpl::write_scissor_box()
{
	return write<Ren3dCmdScissorBox>(Ren3dCmdId::scissor_set_box);
}

Ren3dCmdCulling* Ren3dCmdBufferImpl::write_culling()
{
	return write<Ren3dCmdCulling>(Ren3dCmdId::culling);
}

Ren3dCmdDepthTest* Ren3dCmdBufferImpl::write_depth_test()
{
	return write<Ren3dCmdDepthTest>(Ren3dCmdId::depth_set_test);
}

Ren3dCmdDepthWrite* Ren3dCmdBufferImpl::write_depth_write()
{
	return write<Ren3dCmdDepthWrite>(Ren3dCmdId::depth_set_write);
}

Ren3dCmdBlending* Ren3dCmdBufferImpl::write_blending()
{
	return write<Ren3dCmdBlending>(Ren3dCmdId::blending);
}

Ren3dCmdBlendingFunc* Ren3dCmdBufferImpl::write_blending_func()
{
	return write<Ren3dCmdBlendingFunc>(Ren3dCmdId::blending_func);
}

Ren3dCmdTexture* Ren3dCmdBufferImpl::write_texture()
{
	return write<Ren3dCmdTexture>(Ren3dCmdId::texture);
}

Ren3dCmdSampler* Ren3dCmdBufferImpl::write_sampler()
{
	return write<Ren3dCmdSampler>(Ren3dCmdId::sampler);
}

Ren3dCmdVertexInput* Ren3dCmdBufferImpl::write_vertex_input()
{
	return write<Ren3dCmdVertexInput>(Ren3dCmdId::vertex_input);
}

Ren3dCmdShaderStage* Ren3dCmdBufferImpl::write_shader_stage()
{
	return write<Ren3dCmdShaderStage>(Ren3dCmdId::shader_stage);
}

Ren3dCmdShaderVarInt32* Ren3dCmdBufferImpl::write_shader_var_int32()
{
	return write<Ren3dCmdShaderVarInt32>(Ren3dCmdId::shader_var_int32);
}

Ren3dCmdShaderVarFloat32* Ren3dCmdBufferImpl::write_shader_var_float32()
{
	return write<Ren3dCmdShaderVarFloat32>(Ren3dCmdId::shader_var_float32);
}

Ren3dCmdShaderVarVec2* Ren3dCmdBufferImpl::write_shader_var_vec2()
{
	return write<Ren3dCmdShaderVarVec2>(Ren3dCmdId::shader_var_vec2);
}

Ren3dCmdShaderVarVec4* Ren3dCmdBufferImpl::write_shader_var_vec4()
{
	return write<Ren3dCmdShaderVarVec4>(Ren3dCmdId::shader_var_vec4);
}

Ren3dCmdShaderVarMat4* Ren3dCmdBufferImpl::write_shader_var_mat4()
{
	return write<Ren3dCmdShaderVarMat4>(Ren3dCmdId::shader_var_mat4);
}

Ren3dCmdShaderVarSampler2d* Ren3dCmdBufferImpl::write_shader_var_sampler_2d()
{
	return write<Ren3dCmdShaderVarSampler2d>(Ren3dCmdId::shader_var_sampler_2d);
}

Ren3dCmdDrawIndexed* Ren3dCmdBufferImpl::write_draw_indexed()
{
	return write<Ren3dCmdDrawIndexed>(Ren3dCmdId::draw_indexed);
}

void Ren3dCmdBufferImpl::read_begin()
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

void Ren3dCmdBufferImpl::read_end()
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

Ren3dCmdId Ren3dCmdBufferImpl::read_command_id()
{
	const auto command_id = read<Ren3dCmdId>();

	if (!command_id)
	{
		return Ren3dCmdId::none;
	}

	return *command_id;
}

const Ren3dCmdClear* Ren3dCmdBufferImpl::read_clear()
{
	return read<Ren3dCmdClear>();
}

const Ren3dCmdViewport* Ren3dCmdBufferImpl::read_viewport()
{
	return read<Ren3dCmdViewport>();
}

const Ren3dCmdScissor* Ren3dCmdBufferImpl::read_scissor()
{
	return read<Ren3dCmdScissor>();
}

const Ren3dCmdScissorBox* Ren3dCmdBufferImpl::read_scissor_box()
{
	return read<Ren3dCmdScissorBox>();
}

const Ren3dCmdCulling* Ren3dCmdBufferImpl::read_culling()
{
	return read<Ren3dCmdCulling>();
}

const Ren3dCmdDepthTest* Ren3dCmdBufferImpl::read_depth_test()
{
	return read<Ren3dCmdDepthTest>();
}

const Ren3dCmdDepthWrite* Ren3dCmdBufferImpl::read_depth_write()
{
	return read<Ren3dCmdDepthWrite>();
}

const Ren3dCmdBlending* Ren3dCmdBufferImpl::read_blending()
{
	return read<Ren3dCmdBlending>();
}

const Ren3dCmdBlendingFunc* Ren3dCmdBufferImpl::read_blending_func()
{
	return read<Ren3dCmdBlendingFunc>();
}

const Ren3dCmdTexture* Ren3dCmdBufferImpl::read_texture()
{
	return read<Ren3dCmdTexture>();
}

const Ren3dCmdSampler* Ren3dCmdBufferImpl::read_sampler()
{
	return read<Ren3dCmdSampler>();
}

const Ren3dCmdVertexInput* Ren3dCmdBufferImpl::read_vertex_input()
{
	return read<Ren3dCmdVertexInput>();
}

const Ren3dCmdShaderStage* Ren3dCmdBufferImpl::read_shader_stage()
{
	return read<Ren3dCmdShaderStage>();
}

const Ren3dCmdShaderVarInt32* Ren3dCmdBufferImpl::read_shader_var_int32()
{
	return read<Ren3dCmdShaderVarInt32>();
}

const Ren3dCmdShaderVarFloat32* Ren3dCmdBufferImpl::read_shader_var_float32()
{
	return read<Ren3dCmdShaderVarFloat32>();
}

const Ren3dCmdShaderVarVec2* Ren3dCmdBufferImpl::read_shader_var_vec2()
{
	return read<Ren3dCmdShaderVarVec2>();
}

const Ren3dCmdShaderVarVec4* Ren3dCmdBufferImpl::read_shader_var_vec4()
{
	return read<Ren3dCmdShaderVarVec4>();
}

const Ren3dCmdShaderVarMat4* Ren3dCmdBufferImpl::read_shader_var_mat4()
{
	return read<Ren3dCmdShaderVarMat4>();
}

const Ren3dCmdShaderVarSampler2d* Ren3dCmdBufferImpl::read_shader_var_sampler_2d()
{
	return read<Ren3dCmdShaderVarSampler2d>();
}

const Ren3dCmdDrawIndexed* Ren3dCmdBufferImpl::read_draw_indexed()
{
	return read<Ren3dCmdDrawIndexed>();
}

void Ren3dCmdBufferImpl::validate_param(
	const Ren3dCmdQueueEnqueueParam& param)
{
	if (param.initial_size_ <= 0)
	{
		throw Ren3dCmdBufferImplInitException{"Initial size out of range."};
	}

	if (param.resize_delta_size_ < 0)
	{
		throw Ren3dCmdBufferImplInitException{"Resize delta out of range."};
	}
}

void Ren3dCmdBufferImpl::resize_if_necessary(
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
// Ren3dCmdBufferImpl
// ==========================================================================


} // detail
} // bstone
