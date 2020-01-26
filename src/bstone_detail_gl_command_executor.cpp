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
// OpenGL command executor (implementation).
//
// !!! Internal usage only !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_command_executor.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d.h"
#include "bstone_renderer_3d_tests.h"

#include "bstone_detail_gl_buffer.h"
#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_sampler_manager.h"
#include "bstone_detail_gl_shader_stage.h"
#include "bstone_detail_gl_texture_manager.h"
#include "bstone_detail_gl_vertex_input.h"
#include "bstone_detail_gl_vertex_input_manager.h"
#include "bstone_detail_renderer_3d_command_manager.h"


namespace bstone
{
namespace detail
{


// =========================================================================
// GlCommandExecutor
//

GlCommandExecutor::GlCommandExecutor() = default;

GlCommandExecutor::~GlCommandExecutor() = default;

//
// GlCommandExecutor
// =========================================================================


// =========================================================================
// GlCommandExecutorImpl
//

class GlCommandExecutorImpl final :
	public GlCommandExecutor
{
public:
	GlCommandExecutorImpl(
		const GlContextPtr gl_context);

	~GlCommandExecutorImpl() override;


	void execute(
		const bstone::Renderer3dCommandManagerPtr command_manager) override;


private:
	const GlContextPtr gl_context_;


	void initialize();


	void command_execute_clear(
		const Renderer3dCommandClear& command);

	void command_execute_culling(
		const Renderer3dCommandCulling& command);

	void command_execute_depth_test(
		const Renderer3dCommandDepthTest& command);

	void command_execute_depth_write(
		const Renderer3dCommandDepthWrite& command);

	void command_execute_blending(
		const Renderer3dCommandBlending& command);

	void command_execute_blending_func(
		const Renderer3dCommandBlendingFunc& command);

	void command_execute_viewport(
		const Renderer3dCommandViewport& command);

	void command_execute_scissor(
		const Renderer3dCommandScissor& command);

	void command_execute_scissor_box(
		const Renderer3dCommandScissorBox& command);

	void command_execute_texture(
		const Renderer3dCommandTexture& command);

	void command_execute_sampler(
		const Renderer3dCommandSampler& command);

	void command_execute_vertex_input(
		const Renderer3dCommandVertexInput& command);

	void command_execute_shader_stage(
		const Renderer3dCommandShaderStage& command);

	void command_execute_shader_var_int32(
		const Renderer3dCommandShaderVarInt32& command);

	void command_execute_shader_var_float32(
		const Renderer3dCommandShaderVarFloat32& command);

	void command_execute_shader_var_vec2(
		const Renderer3dCommandShaderVarVec2& command);

	void command_execute_shader_var_vec4(
		const Renderer3dCommandShaderVarVec4& command);

	void command_execute_shader_var_mat4(
		const Renderer3dCommandShaderVarMat4& command);

	void command_execute_shader_var_sampler_2d(
		const Renderer3dCommandShaderVarSampler2d& command);

	void command_execute_draw_indexed(
		const Renderer3dCommandDrawIndexed& command);
}; // GlCommandExecutorImpl


using GlCommandExecutorImplUPtr = std::unique_ptr<GlCommandExecutorImpl>;

//
// GlCommandExecutorImpl
// =========================================================================


// =========================================================================
// GlCommandExecutorImpl
//

GlCommandExecutorImpl::GlCommandExecutorImpl(
	const GlContextPtr gl_context)
	:
	gl_context_{gl_context}
{
	initialize();
}

GlCommandExecutorImpl::~GlCommandExecutorImpl() = default;

void GlCommandExecutorImpl::execute(
	const bstone::Renderer3dCommandManagerPtr command_manager)
{
	if (!command_manager)
	{
		throw Exception{"Null command manager."};
	}

	const auto buffer_count = command_manager->buffer_get_count();

	for (int i = 0; i < buffer_count; ++i)
	{
		auto command_buffer = command_manager->buffer_get(i);

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

void GlCommandExecutorImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL context."};
	}
}

void GlCommandExecutorImpl::command_execute_clear(
	const Renderer3dCommandClear& command)
{
	gl_context_->clear(command.param_.color_);
}

void GlCommandExecutorImpl::command_execute_culling(
	const Renderer3dCommandCulling& command)
{
	gl_context_->culling_enable(command.is_enable_);
}

void GlCommandExecutorImpl::command_execute_depth_test(
	const Renderer3dCommandDepthTest& command)
{
	gl_context_->depth_test_enable(command.is_enable_);
}

void GlCommandExecutorImpl::command_execute_depth_write(
	const Renderer3dCommandDepthWrite& command)
{
	gl_context_->depth_write_enable(command.is_enable_);
}

void GlCommandExecutorImpl::command_execute_viewport(
	const Renderer3dCommandViewport& command)
{
	gl_context_->viewport_set(command.viewport_);
}

void GlCommandExecutorImpl::command_execute_blending(
	const Renderer3dCommandBlending& command)
{
	gl_context_->blending_enable(command.is_enable_);
}

void GlCommandExecutorImpl::command_execute_blending_func(
	const Renderer3dCommandBlendingFunc& command)
{
	gl_context_->blending_set_func(command.blending_func_);
}

void GlCommandExecutorImpl::command_execute_scissor(
	const Renderer3dCommandScissor& command)
{
	gl_context_->scissor_enable(command.is_enable_);
}

void GlCommandExecutorImpl::command_execute_scissor_box(
	const Renderer3dCommandScissorBox& command)
{
	gl_context_->scissor_set_box(command.scissor_box_);
}

void GlCommandExecutorImpl::command_execute_texture(
	const Renderer3dCommandTexture& command)
{
	gl_context_->texture_get_manager()->set(command.texture_2d_);
}

void GlCommandExecutorImpl::command_execute_sampler(
	const Renderer3dCommandSampler& command)
{
	gl_context_->sampler_get_manager()->set(command.sampler_);
}

void GlCommandExecutorImpl::command_execute_vertex_input(
	const Renderer3dCommandVertexInput& command)
{
	auto vertex_input = static_cast<GlVertexInputPtr>(command.vertex_input_);

	gl_context_->vertex_input_get_manager()->set(vertex_input);
}

void GlCommandExecutorImpl::command_execute_shader_stage(
	const Renderer3dCommandShaderStage& command)
{
	if (!command.shader_stage_)
	{
		throw Exception{"Null shader stage."};
	}

	static_cast<GlShaderStagePtr>(command.shader_stage_)->set();
}

void GlCommandExecutorImpl::command_execute_shader_var_int32(
	const Renderer3dCommandShaderVarInt32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_int(command.value_);
}

void GlCommandExecutorImpl::command_execute_shader_var_float32(
	const Renderer3dCommandShaderVarFloat32& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_float(command.value_);
}

void GlCommandExecutorImpl::command_execute_shader_var_vec2(
	const Renderer3dCommandShaderVarVec2& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec2(command.value_.data());
}

void GlCommandExecutorImpl::command_execute_shader_var_vec4(
	const Renderer3dCommandShaderVarVec4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_vec4(command.value_.data());
}

void GlCommandExecutorImpl::command_execute_shader_var_mat4(
	const Renderer3dCommandShaderVarMat4& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_mat4(command.value_.data());
}

void GlCommandExecutorImpl::command_execute_shader_var_sampler_2d(
	const Renderer3dCommandShaderVarSampler2d& command)
{
	if (!command.var_)
	{
		throw Exception{"Null variable."};
	}

	command.var_->set_sampler_2d(command.value_);
}

void GlCommandExecutorImpl::command_execute_draw_indexed(
	const Renderer3dCommandDrawIndexed& command)
{
	const auto& param = command.param_;

	auto gl_primitive_topology = GLenum{};

	switch (param.primitive_topology_)
	{
		case Renderer3dPrimitiveTopology::point_list:
			gl_primitive_topology = GL_POINTS;
			break;

		case Renderer3dPrimitiveTopology::line_list:
			gl_primitive_topology = GL_LINES;
			break;

		case Renderer3dPrimitiveTopology::line_strip:
			gl_primitive_topology = GL_LINE_STRIP;
			break;

		case Renderer3dPrimitiveTopology::triangle_list:
			gl_primitive_topology = GL_TRIANGLES;
			break;

		case Renderer3dPrimitiveTopology::triangle_strip:
			gl_primitive_topology = GL_TRIANGLE_STRIP;
			break;

		default:
			throw Exception{"Unsupported primitive topology."};
	}

	if (param.vertex_count_ < 0)
	{
		throw Exception{"Vertex count out of range."};
	}

	if (param.vertex_count_ == 0)
	{
		return;
	}

	switch (param.index_byte_depth_)
	{
		case 1:
		case 2:
		case 4:
			break;

		default:
			throw Exception{"Unsupported index value byte depth."};
	}

	if (param.index_buffer_offset_ < 0)
	{
		throw Exception{"Offset to indices out of range."};
	}

	if (param.index_offset_ < 0)
	{
		throw Exception{"Index offset out of range."};
	}

	auto index_buffer = static_cast<GlBufferPtr>(gl_context_->vertex_input_get_manager()->get_current_index_buffer());

	if (!index_buffer)
	{
		throw Exception{"Null index buffer."};
	}

	const auto index_buffer_offset = param.index_buffer_offset_ + (param.index_offset_ * param.index_byte_depth_);
	const auto index_buffer_indices = reinterpret_cast<const void*>(static_cast<std::intptr_t>(index_buffer_offset));

	const auto gl_element_type = GlRenderer3dUtils::index_buffer_get_element_type_by_byte_depth(
		param.index_byte_depth_);

	index_buffer->set(true);

	glDrawElements(
		gl_primitive_topology, // mode
		param.vertex_count_, // count
		gl_element_type, // type
		index_buffer_indices // indices
	);

	assert(!GlRenderer3dUtils::was_errors());
}

//
// GlCommandExecutorImpl
// =========================================================================


// =========================================================================
// GlCommandExecutorFactory
//

GlCommandExecutorUPtr GlCommandExecutorFactory::create(
	const GlContextPtr gl_context)
{
	return std::make_unique<GlCommandExecutorImpl>(gl_context);
}

//
// GlCommandExecutorFactory
// =========================================================================


} // detail
} // bstone
