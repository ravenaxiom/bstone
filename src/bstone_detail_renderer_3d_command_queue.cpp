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
// 3D renderer's command manager (implementaion).
//


#include "bstone_precompiled.h"
#include "bstone_detail_renderer_3d_command_queue.h"

#include <algorithm>

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandQueueImpl
//

Renderer3dCommandQueueImpl::Renderer3dCommandQueueImpl(
	const Renderer3dPtr renderer_3d)
	:
	renderer_3d_{renderer_3d},
	buffers_{Renderer3dCommandBuffersFactory::create()}
{
}

Renderer3dCommandQueueImpl::~Renderer3dCommandQueueImpl() = default;

int Renderer3dCommandQueueImpl::buffer_get_count() const noexcept
{
	return buffers_->buffer_get_count();
}

bstone::Renderer3dCommandBufferPtr Renderer3dCommandQueueImpl::buffer_add(
	const Renderer3dCommandQueueAddBufferParam& param)
{
	return buffers_->buffer_add(param);
}

void Renderer3dCommandQueueImpl::buffer_remove(
	bstone::Renderer3dCommandBufferPtr buffer)
{
	buffers_->buffer_remove(buffer);
}

bstone::Renderer3dCommandBufferPtr Renderer3dCommandQueueImpl::buffer_get(
	const int index)
{
	return buffers_->buffer_get(index);
}

void Renderer3dCommandQueueImpl::validate_param(
	const Renderer3dCommandQueueAddBufferParam& param)
{
	if (param.initial_size_ < 0)
	{
		throw Exception{"Initial size out of range."};
	}

	if (param.resize_delta_size_ < 0)
	{
		throw Exception{"Resize delta out of range."};
	}
}

void Renderer3dCommandQueueImpl::command_execute_clear(
	const Renderer3dCommandClear& command)
{
	renderer_3d_->clear(command.param_);
}

void Renderer3dCommandQueueImpl::command_execute_culling(
	const Renderer3dCommandCulling& command)
{
	renderer_3d_->culling_enable(command.is_enable_);
}

void Renderer3dCommandQueueImpl::command_execute_depth_test(
	const Renderer3dCommandDepthTest& command)
{
	renderer_3d_->depth_test_enable(command.is_enable_);
}

void Renderer3dCommandQueueImpl::command_execute_depth_write(
	const Renderer3dCommandDepthWrite& command)
{
	renderer_3d_->depth_write_enable(command.is_enable_);
}

void Renderer3dCommandQueueImpl::command_execute_viewport(
	const Renderer3dCommandViewport& command)
{
	renderer_3d_->viewport_set(command.viewport_);
}

void Renderer3dCommandQueueImpl::command_execute_blending(
	const Renderer3dCommandBlending& command)
{
	renderer_3d_->blending_enable(command.is_enable_);
}

void Renderer3dCommandQueueImpl::command_execute_blending_func(
	const Renderer3dCommandBlendingFunc& command)
{
	renderer_3d_->blending_function_set(command.blending_func_);
}

void Renderer3dCommandQueueImpl::command_execute_scissor(
	const Renderer3dCommandScissor& command)
{
	renderer_3d_->scissor_enable(command.is_enable_);
}

void Renderer3dCommandQueueImpl::command_execute_scissor_box(
	const Renderer3dCommandScissorBox& command)
{
	renderer_3d_->scissor_box_set(command.scissor_box_);
}

void Renderer3dCommandQueueImpl::command_execute_texture(
	const Renderer3dCommandTexture& command)
{
	renderer_3d_->texture_2d_set(command.texture_2d_);
}

void Renderer3dCommandQueueImpl::command_execute_sampler(
	const Renderer3dCommandSampler& command)
{
	renderer_3d_->sampler_set(command.sampler_);
}

void Renderer3dCommandQueueImpl::command_execute_vertex_input(
	const Renderer3dCommandVertexInput& command)
{
	renderer_3d_->vertex_input_set(command.vertex_input_);
}

void Renderer3dCommandQueueImpl::command_execute_shader_stage(
	const Renderer3dCommandShaderStage& command)
{
	renderer_3d_->shader_stage_set(command.shader_stage_);
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_int32(
	const Renderer3dCommandShaderVarInt32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_int(command.value_);
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_float32(
	const Renderer3dCommandShaderVarFloat32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_float(command.value_);
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_vec2(
	const Renderer3dCommandShaderVarVec2& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec2(command.value_.data());
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_vec4(
	const Renderer3dCommandShaderVarVec4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec4(command.value_.data());
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_mat4(
	const Renderer3dCommandShaderVarMat4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_mat4(command.value_.data());
}

void Renderer3dCommandQueueImpl::command_execute_shader_var_sampler_2d(
	const Renderer3dCommandShaderVarSampler2d& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_sampler_2d(command.value_);
}

void Renderer3dCommandQueueImpl::command_execute_draw_indexed(
	const Renderer3dCommandDrawIndexed& command)
{
	renderer_3d_->draw_indexed(command.param_);
}

void Renderer3dCommandQueueImpl::command_execute()
{
	const auto buffer_count = buffer_get_count();

	for (auto& command_buffer : *buffers_)
	{
		if (!command_buffer->is_enabled())
		{
			continue;
		}

		const auto command_count = command_buffer->get_command_count();

		command_buffer->read_begin();

		for (int j = 0; j < command_count; ++j)
		{
			const auto command_id = command_buffer->read_command_id();

			switch (command_id)
			{
			case Renderer3dCommandId::clear:
				command_execute_clear(*command_buffer->read_clear());
				break;

			case Renderer3dCommandId::culling:
				command_execute_culling(*command_buffer->read_culling());
				break;

			case Renderer3dCommandId::depth_set_test:
				command_execute_depth_test(*command_buffer->read_depth_test());
				break;

			case Renderer3dCommandId::depth_set_write:
				command_execute_depth_write(*command_buffer->read_depth_write());
				break;

			case Renderer3dCommandId::viewport:
				command_execute_viewport(*command_buffer->read_viewport());
				break;

			case Renderer3dCommandId::scissor:
				command_execute_scissor(*command_buffer->read_scissor());
				break;

			case Renderer3dCommandId::scissor_set_box:
				command_execute_scissor_box(*command_buffer->read_scissor_box());
				break;

			case Renderer3dCommandId::blending:
				command_execute_blending(*command_buffer->read_blending());
				break;

			case Renderer3dCommandId::blending_func:
				command_execute_blending_func(*command_buffer->read_blending_func());
				break;

			case Renderer3dCommandId::texture:
				command_execute_texture(*command_buffer->read_texture());
				break;

			case Renderer3dCommandId::sampler:
				command_execute_sampler(*command_buffer->read_sampler());
				break;

			case Renderer3dCommandId::vertex_input:
				command_execute_vertex_input(*command_buffer->read_vertex_input());
				break;

			case Renderer3dCommandId::shader_stage:
				command_execute_shader_stage(*command_buffer->read_shader_stage());
				break;

			case Renderer3dCommandId::shader_var_int32:
				command_execute_shader_var_int32(*command_buffer->read_shader_var_int32());
				break;

			case Renderer3dCommandId::shader_var_float32:
				command_execute_shader_var_float32(*command_buffer->read_shader_var_float32());
				break;

			case Renderer3dCommandId::shader_var_vec2:
				command_execute_shader_var_vec2(*command_buffer->read_shader_var_vec2());
				break;

			case Renderer3dCommandId::shader_var_vec4:
				command_execute_shader_var_vec4(*command_buffer->read_shader_var_vec4());
				break;

			case Renderer3dCommandId::shader_var_mat4:
				command_execute_shader_var_mat4(*command_buffer->read_shader_var_mat4());
				break;

			case Renderer3dCommandId::shader_var_sampler_2d:
				command_execute_shader_var_sampler_2d(*command_buffer->read_shader_var_sampler_2d());
				break;

			case Renderer3dCommandId::draw_indexed:
				command_execute_draw_indexed(*command_buffer->read_draw_indexed());
				break;

			default:
				throw Exception{"Unsupported command id."};
			}
		}

		command_buffer->read_end();
	}
}

//
// Renderer3dCommandQueueImpl
// ==========================================================================


} // detail


// ==========================================================================
// Renderer3dCommandQueueFactory
//

Renderer3dCommandQueueUPtr Renderer3dCommandQueueFactory::create(
	const Renderer3dPtr renderer_3d)
{
	return std::make_unique<detail::Renderer3dCommandQueueImpl>(renderer_3d);
}

//
// Renderer3dCommandQueueFactory
// ==========================================================================


} // bstone
