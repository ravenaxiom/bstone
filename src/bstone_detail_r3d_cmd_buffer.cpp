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
// R3dCmdBufferImplInitException
//

class R3dCmdBufferImplInitException :
	public Exception
{
public:
	explicit R3dCmdBufferImplInitException(
		const char* const message)
		:
		Exception{std::string{"[R3D_CMD_BUF_INIT] "} + message}
	{
	}
}; // R3dCmdBufferImplInitException

//
// R3dCmdBufferImplInitException
// ==========================================================================


// ==========================================================================
// R3dCmdBufferImpl
//

R3dCmdBufferImpl::R3dCmdBufferImpl(
	const R3dCmdQueueEnqueueParam& param)
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

R3dCmdBufferImpl::~R3dCmdBufferImpl() = default;

int R3dCmdBufferImpl::get_command_count() const noexcept
{
	return command_count_;
}

bool R3dCmdBufferImpl::is_enabled() const noexcept
{
	return is_enabled_;
}

void R3dCmdBufferImpl::enable(
	const bool is_enabled)
{
	is_enabled_ = is_enabled;
}

void R3dCmdBufferImpl::write_begin()
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

void R3dCmdBufferImpl::write_end()
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

R3dCmdClear* R3dCmdBufferImpl::write_clear()
{
	return write<R3dCmdClear>(R3dCmdId::clear);
}

R3dCmdViewport* R3dCmdBufferImpl::write_viewport()
{
	return write<R3dCmdViewport>(R3dCmdId::viewport);
}

R3dCmdScissor* R3dCmdBufferImpl::write_scissor()
{
	return write<R3dCmdScissor>(R3dCmdId::scissor);
}

R3dCmdScissorBox* R3dCmdBufferImpl::write_scissor_box()
{
	return write<R3dCmdScissorBox>(R3dCmdId::scissor_set_box);
}

R3dCmdCulling* R3dCmdBufferImpl::write_culling()
{
	return write<R3dCmdCulling>(R3dCmdId::culling);
}

R3dCmdDepthTest* R3dCmdBufferImpl::write_depth_test()
{
	return write<R3dCmdDepthTest>(R3dCmdId::depth_set_test);
}

R3dCmdDepthWrite* R3dCmdBufferImpl::write_depth_write()
{
	return write<R3dCmdDepthWrite>(R3dCmdId::depth_set_write);
}

R3dCmdBlending* R3dCmdBufferImpl::write_blending()
{
	return write<R3dCmdBlending>(R3dCmdId::blending);
}

R3dCmdBlendingFunc* R3dCmdBufferImpl::write_blending_func()
{
	return write<R3dCmdBlendingFunc>(R3dCmdId::blending_func);
}

R3dCmdTexture* R3dCmdBufferImpl::write_texture()
{
	return write<R3dCmdTexture>(R3dCmdId::texture);
}

R3dCmdSampler* R3dCmdBufferImpl::write_sampler()
{
	return write<R3dCmdSampler>(R3dCmdId::sampler);
}

R3dCmdVertexInput* R3dCmdBufferImpl::write_vertex_input()
{
	return write<R3dCmdVertexInput>(R3dCmdId::vertex_input);
}

R3dCmdShaderStage* R3dCmdBufferImpl::write_shader_stage()
{
	return write<R3dCmdShaderStage>(R3dCmdId::shader_stage);
}

R3dCmdShaderVarInt32* R3dCmdBufferImpl::write_shader_var_int32()
{
	return write<R3dCmdShaderVarInt32>(R3dCmdId::shader_var_int32);
}

R3dCmdShaderVarFloat32* R3dCmdBufferImpl::write_shader_var_float32()
{
	return write<R3dCmdShaderVarFloat32>(R3dCmdId::shader_var_float32);
}

R3dCmdShaderVarVec2* R3dCmdBufferImpl::write_shader_var_vec2()
{
	return write<R3dCmdShaderVarVec2>(R3dCmdId::shader_var_vec2);
}

R3dCmdShaderVarVec4* R3dCmdBufferImpl::write_shader_var_vec4()
{
	return write<R3dCmdShaderVarVec4>(R3dCmdId::shader_var_vec4);
}

R3dCmdShaderVarMat4* R3dCmdBufferImpl::write_shader_var_mat4()
{
	return write<R3dCmdShaderVarMat4>(R3dCmdId::shader_var_mat4);
}

R3dCmdShaderVarSampler2d* R3dCmdBufferImpl::write_shader_var_sampler_2d()
{
	return write<R3dCmdShaderVarSampler2d>(R3dCmdId::shader_var_sampler_2d);
}

R3dCmdDrawIndexed* R3dCmdBufferImpl::write_draw_indexed()
{
	return write<R3dCmdDrawIndexed>(R3dCmdId::draw_indexed);
}

void R3dCmdBufferImpl::read_begin()
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

void R3dCmdBufferImpl::read_end()
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

R3dCmdId R3dCmdBufferImpl::read_command_id()
{
	const auto command_id = read<R3dCmdId>();

	if (!command_id)
	{
		return R3dCmdId::none;
	}

	return *command_id;
}

const R3dCmdClear* R3dCmdBufferImpl::read_clear()
{
	return read<R3dCmdClear>();
}

const R3dCmdViewport* R3dCmdBufferImpl::read_viewport()
{
	return read<R3dCmdViewport>();
}

const R3dCmdScissor* R3dCmdBufferImpl::read_scissor()
{
	return read<R3dCmdScissor>();
}

const R3dCmdScissorBox* R3dCmdBufferImpl::read_scissor_box()
{
	return read<R3dCmdScissorBox>();
}

const R3dCmdCulling* R3dCmdBufferImpl::read_culling()
{
	return read<R3dCmdCulling>();
}

const R3dCmdDepthTest* R3dCmdBufferImpl::read_depth_test()
{
	return read<R3dCmdDepthTest>();
}

const R3dCmdDepthWrite* R3dCmdBufferImpl::read_depth_write()
{
	return read<R3dCmdDepthWrite>();
}

const R3dCmdBlending* R3dCmdBufferImpl::read_blending()
{
	return read<R3dCmdBlending>();
}

const R3dCmdBlendingFunc* R3dCmdBufferImpl::read_blending_func()
{
	return read<R3dCmdBlendingFunc>();
}

const R3dCmdTexture* R3dCmdBufferImpl::read_texture()
{
	return read<R3dCmdTexture>();
}

const R3dCmdSampler* R3dCmdBufferImpl::read_sampler()
{
	return read<R3dCmdSampler>();
}

const R3dCmdVertexInput* R3dCmdBufferImpl::read_vertex_input()
{
	return read<R3dCmdVertexInput>();
}

const R3dCmdShaderStage* R3dCmdBufferImpl::read_shader_stage()
{
	return read<R3dCmdShaderStage>();
}

const R3dCmdShaderVarInt32* R3dCmdBufferImpl::read_shader_var_int32()
{
	return read<R3dCmdShaderVarInt32>();
}

const R3dCmdShaderVarFloat32* R3dCmdBufferImpl::read_shader_var_float32()
{
	return read<R3dCmdShaderVarFloat32>();
}

const R3dCmdShaderVarVec2* R3dCmdBufferImpl::read_shader_var_vec2()
{
	return read<R3dCmdShaderVarVec2>();
}

const R3dCmdShaderVarVec4* R3dCmdBufferImpl::read_shader_var_vec4()
{
	return read<R3dCmdShaderVarVec4>();
}

const R3dCmdShaderVarMat4* R3dCmdBufferImpl::read_shader_var_mat4()
{
	return read<R3dCmdShaderVarMat4>();
}

const R3dCmdShaderVarSampler2d* R3dCmdBufferImpl::read_shader_var_sampler_2d()
{
	return read<R3dCmdShaderVarSampler2d>();
}

const R3dCmdDrawIndexed* R3dCmdBufferImpl::read_draw_indexed()
{
	return read<R3dCmdDrawIndexed>();
}

void R3dCmdBufferImpl::validate_param(
	const R3dCmdQueueEnqueueParam& param)
{
	if (param.initial_size_ <= 0)
	{
		throw R3dCmdBufferImplInitException{"Initial size out of range."};
	}

	if (param.resize_delta_size_ < 0)
	{
		throw R3dCmdBufferImplInitException{"Resize delta out of range."};
	}
}

void R3dCmdBufferImpl::resize_if_necessary(
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
// R3dCmdBufferImpl
// ==========================================================================


} // detail
} // bstone
