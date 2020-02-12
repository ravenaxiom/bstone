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

int Ren3dCmdBufferImpl::get_count() const noexcept
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

void Ren3dCmdBufferImpl::begin_write()
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

void Ren3dCmdBufferImpl::end_write()
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

Ren3dClearCmd* Ren3dCmdBufferImpl::write_clear()
{
	return write<Ren3dClearCmd>(Ren3dCmdId::clear);
}

Ren3dSetViewportCmd* Ren3dCmdBufferImpl::write_set_viewport()
{
	return write<Ren3dSetViewportCmd>(Ren3dCmdId::viewport);
}

Ren3dEnableScissorCmd* Ren3dCmdBufferImpl::write_enable_scissor()
{
	return write<Ren3dEnableScissorCmd>(Ren3dCmdId::scissor);
}

Ren3dSetScissorBoxCmd* Ren3dCmdBufferImpl::write_set_scissor_box()
{
	return write<Ren3dSetScissorBoxCmd>(Ren3dCmdId::scissor_set_box);
}

Ren3dEnableCullingCmd* Ren3dCmdBufferImpl::write_enable_culling()
{
	return write<Ren3dEnableCullingCmd>(Ren3dCmdId::culling);
}

Ren3dEnableDepthTestCmd* Ren3dCmdBufferImpl::write_enable_depth_test()
{
	return write<Ren3dEnableDepthTestCmd>(Ren3dCmdId::depth_set_test);
}

Ren3dEnableDepthWriteCmd* Ren3dCmdBufferImpl::write_enable_depth_write()
{
	return write<Ren3dEnableDepthWriteCmd>(Ren3dCmdId::depth_set_write);
}

Ren3dEnableBlendingCmd* Ren3dCmdBufferImpl::write_enable_blending()
{
	return write<Ren3dEnableBlendingCmd>(Ren3dCmdId::blending);
}

Ren3dSetBlendingFuncCmd* Ren3dCmdBufferImpl::write_set_blending_func()
{
	return write<Ren3dSetBlendingFuncCmd>(Ren3dCmdId::blending_func);
}

Ren3dSetTextureCmd* Ren3dCmdBufferImpl::write_set_texture()
{
	return write<Ren3dSetTextureCmd>(Ren3dCmdId::texture);
}

Ren3dSetSamplerCmd* Ren3dCmdBufferImpl::write_set_sampler()
{
	return write<Ren3dSetSamplerCmd>(Ren3dCmdId::sampler);
}

Ren3dSetVertexInputCmd* Ren3dCmdBufferImpl::write_set_vertex_input()
{
	return write<Ren3dSetVertexInputCmd>(Ren3dCmdId::vertex_input);
}

Ren3dSetShaderStageCmd* Ren3dCmdBufferImpl::write_set_shader_stage()
{
	return write<Ren3dSetShaderStageCmd>(Ren3dCmdId::shader_stage);
}

Ren3dSetInt32UniformCmd* Ren3dCmdBufferImpl::write_set_int32_uniform()
{
	return write<Ren3dSetInt32UniformCmd>(Ren3dCmdId::shader_var_int32);
}

Ren3dSetFloat32UniformCmd* Ren3dCmdBufferImpl::write_set_float32_uniform()
{
	return write<Ren3dSetFloat32UniformCmd>(Ren3dCmdId::shader_var_float32);
}

Ren3dSetVec2UniformCmd* Ren3dCmdBufferImpl::write_set_vec2_uniform()
{
	return write<Ren3dSetVec2UniformCmd>(Ren3dCmdId::shader_var_vec2);
}

Ren3dSetVec4UniformCmd* Ren3dCmdBufferImpl::write_set_vec4_uniform()
{
	return write<Ren3dSetVec4UniformCmd>(Ren3dCmdId::shader_var_vec4);
}

Ren3dSetMat4UniformCmd* Ren3dCmdBufferImpl::write_set_mat4_uniform()
{
	return write<Ren3dSetMat4UniformCmd>(Ren3dCmdId::shader_var_mat4);
}

Ren3dSetSampler2dUniformCmd* Ren3dCmdBufferImpl::write_set_sampler_2d_uniform()
{
	return write<Ren3dSetSampler2dUniformCmd>(Ren3dCmdId::shader_var_sampler_2d);
}

Ren3dDrawIndexedCmd* Ren3dCmdBufferImpl::write_draw_indexed()
{
	return write<Ren3dDrawIndexedCmd>(Ren3dCmdId::draw_indexed);
}

void Ren3dCmdBufferImpl::begin_read()
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

void Ren3dCmdBufferImpl::end_read()
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

const Ren3dClearCmd* Ren3dCmdBufferImpl::read_clear()
{
	return read<Ren3dClearCmd>();
}

const Ren3dSetViewportCmd* Ren3dCmdBufferImpl::read_set_viewport()
{
	return read<Ren3dSetViewportCmd>();
}

const Ren3dEnableScissorCmd* Ren3dCmdBufferImpl::read_enable_scissor()
{
	return read<Ren3dEnableScissorCmd>();
}

const Ren3dSetScissorBoxCmd* Ren3dCmdBufferImpl::read_set_scissor_box()
{
	return read<Ren3dSetScissorBoxCmd>();
}

const Ren3dEnableCullingCmd* Ren3dCmdBufferImpl::read_enable_culling()
{
	return read<Ren3dEnableCullingCmd>();
}

const Ren3dEnableDepthTestCmd* Ren3dCmdBufferImpl::read_enable_depth_test()
{
	return read<Ren3dEnableDepthTestCmd>();
}

const Ren3dEnableDepthWriteCmd* Ren3dCmdBufferImpl::read_enable_depth_write()
{
	return read<Ren3dEnableDepthWriteCmd>();
}

const Ren3dEnableBlendingCmd* Ren3dCmdBufferImpl::read_enable_blending()
{
	return read<Ren3dEnableBlendingCmd>();
}

const Ren3dSetBlendingFuncCmd* Ren3dCmdBufferImpl::read_set_blending_func()
{
	return read<Ren3dSetBlendingFuncCmd>();
}

const Ren3dSetTextureCmd* Ren3dCmdBufferImpl::read_set_texture()
{
	return read<Ren3dSetTextureCmd>();
}

const Ren3dSetSamplerCmd* Ren3dCmdBufferImpl::read_set_sampler()
{
	return read<Ren3dSetSamplerCmd>();
}

const Ren3dSetVertexInputCmd* Ren3dCmdBufferImpl::read_set_vertex_input()
{
	return read<Ren3dSetVertexInputCmd>();
}

const Ren3dSetShaderStageCmd* Ren3dCmdBufferImpl::read_set_shader_stage()
{
	return read<Ren3dSetShaderStageCmd>();
}

const Ren3dSetInt32UniformCmd* Ren3dCmdBufferImpl::read_set_int32_uniform()
{
	return read<Ren3dSetInt32UniformCmd>();
}

const Ren3dSetFloat32UniformCmd* Ren3dCmdBufferImpl::read_set_float32_uniform()
{
	return read<Ren3dSetFloat32UniformCmd>();
}

const Ren3dSetVec2UniformCmd* Ren3dCmdBufferImpl::read_set_vec2_uniform()
{
	return read<Ren3dSetVec2UniformCmd>();
}

const Ren3dSetVec4UniformCmd* Ren3dCmdBufferImpl::read_set_vec4_uniform()
{
	return read<Ren3dSetVec4UniformCmd>();
}

const Ren3dSetMat4UniformCmd* Ren3dCmdBufferImpl::read_set_mat4_uniform()
{
	return read<Ren3dSetMat4UniformCmd>();
}

const Ren3dSetSampler2dUniformCmd* Ren3dCmdBufferImpl::read_set_sampler_2d_uniform()
{
	return read<Ren3dSetSampler2dUniformCmd>();
}

const Ren3dDrawIndexedCmd* Ren3dCmdBufferImpl::read_draw_indexed()
{
	return read<Ren3dDrawIndexedCmd>();
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
