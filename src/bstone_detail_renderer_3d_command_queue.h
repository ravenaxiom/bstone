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
// 3D renderer's command queue (implementaion).
//


#ifndef BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED


#include "bstone_detail_renderer_3d_command_buffer.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandQueueImpl
//

class Renderer3dCommandQueueImpl final :
	public Renderer3dCommandQueue
{
public:
	Renderer3dCommandQueueImpl(
		const Renderer3dPtr renderer_3d);

	~Renderer3dCommandQueueImpl() override;


	int buffer_get_count() const noexcept override;

	bstone::Renderer3dCommandBufferPtr buffer_add(
		const Renderer3dCommandQueueBufferAddParam& param) override;

	void buffer_remove(
		bstone::Renderer3dCommandBufferPtr buffer) override;

	bstone::Renderer3dCommandBufferPtr buffer_get(
		const int index) override;

	void command_execute() override;


private:
	static constexpr auto reserved_buffer_count = 8;


	using Buffers = std::vector<detail::RendererCommandBufferUPtr>;


	const Renderer3dPtr renderer_3d_;

	Buffers buffers_;


	static void validate_param(
		const Renderer3dCommandQueueBufferAddParam& param);


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
}; // Renderer3dCommandQueueImpl

using RendererCommandQueueImplPtr = Renderer3dCommandQueueImpl*;
using RendererCommandQueueImplUPtr = std::unique_ptr<Renderer3dCommandQueueImpl>;

//
// Renderer3dCommandQueueImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED
