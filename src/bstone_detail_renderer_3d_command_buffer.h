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
// 3D renderer's command buffer (implementaion).
//


#ifndef BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFER_INCLUDED
#define BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFER_INCLUDED


#include "bstone_renderer_3d_command_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandBufferImpl
//

class Renderer3dCommandBufferImpl final :
	public Renderer3dCommandBuffer
{
public:
	Renderer3dCommandBufferImpl();

	~Renderer3dCommandBufferImpl() override;


	int get_command_count() const noexcept override;


	bool is_enabled() const noexcept override;

	void enable(
		const bool is_enabled) override;


	void write_begin() override;

	void write_end() override;

	Renderer3dCommandClear* write_clear() override;

	Renderer3dCommandViewport* write_viewport() override;

	Renderer3dCommandScissor* write_scissor() override;
	Renderer3dCommandScissorBox* write_scissor_box() override;

	Renderer3dCommandCulling* write_culling() override;

	Renderer3dCommandDepthTest* write_depth_test() override;
	Renderer3dCommandDepthWrite* write_depth_write() override;

	Renderer3dCommandBlending* write_blending() override;
	Renderer3dCommandBlendingFunc* write_blending_func() override;

	Renderer3dCommandTexture* write_texture() override;
	Renderer3dCommandSampler* write_sampler() override;

	Renderer3dCommandVertexInput* write_vertex_input() override;

	Renderer3dCommandShaderStage* write_shader_stage() override;

	Renderer3dCommandShaderVarInt32* write_shader_var_int32() override;
	Renderer3dCommandShaderVarFloat32* write_shader_var_float32() override;
	Renderer3dCommandShaderVarVec2* write_shader_var_vec2() override;
	Renderer3dCommandShaderVarVec4* write_shader_var_vec4() override;
	Renderer3dCommandShaderVarMat4* write_shader_var_mat4() override;
	Renderer3dCommandShaderVarSampler2d* write_shader_var_sampler_2d() override;

	Renderer3dCommandDrawIndexed* write_draw_indexed() override;


	void read_begin() override;

	void read_end() override;

	Renderer3dCommandId read_command_id() override;

	const Renderer3dCommandClear* read_clear() override;

	const Renderer3dCommandViewport* read_viewport() override;

	const Renderer3dCommandScissor* read_scissor() override;
	const Renderer3dCommandScissorBox* read_scissor_box() override;

	const Renderer3dCommandCulling* read_culling() override;

	const Renderer3dCommandDepthTest* read_depth_test() override;
	const Renderer3dCommandDepthWrite* read_depth_write() override;

	const Renderer3dCommandBlending* read_blending() override;
	const Renderer3dCommandBlendingFunc* read_blending_func() override;

	const Renderer3dCommandTexture* read_texture() override;
	const Renderer3dCommandSampler* read_sampler() override;

	const Renderer3dCommandVertexInput* read_vertex_input() override;

	const Renderer3dCommandShaderStage* read_shader_stage() override;

	const Renderer3dCommandShaderVarInt32* read_shader_var_int32() override;
	const Renderer3dCommandShaderVarFloat32* read_shader_var_float32() override;
	const Renderer3dCommandShaderVarVec2* read_shader_var_vec2() override;
	const Renderer3dCommandShaderVarVec4* read_shader_var_vec4() override;
	const Renderer3dCommandShaderVarMat4* read_shader_var_mat4() override;
	const Renderer3dCommandShaderVarSampler2d* read_shader_var_sampler_2d() override;

	const Renderer3dCommandDrawIndexed* read_draw_indexed() override;


	void initialize(
		const Renderer3dCommandManagerBufferAddParam& param);


private:
	static constexpr int get_min_initial_size()
	{
		return 4096;
	}

	static constexpr int get_min_resize_delta_size()
	{
		return 4096;
	}

	static constexpr int get_command_id_size()
	{
		return static_cast<int>(sizeof(Renderer3dCommandId));
	}


	using Data = std::vector<std::uint8_t>;

	bool is_enabled_;
	bool is_reading_;
	bool is_writing_;

	int size_;
	int write_offset_;
	int read_offset_;
	int resize_delta_size_;
	int command_count_;

	Data data_;


	void resize_if_necessary(
		const int dst_delta_size);

	template<typename T>
	T* write(
		const Renderer3dCommandId command_id)
	{
		if (is_reading_ || !is_writing_)
		{
			assert(!"Invalid state.");

			return nullptr;
		}

		if (command_id == Renderer3dCommandId::none)
		{
			assert(!"Invalid command id.");

			return nullptr;
		}

		const auto command_size = static_cast<int>(sizeof(T));

		const auto block_size = get_command_id_size() + command_size;

		resize_if_necessary(block_size);

		auto block = reinterpret_cast<Renderer3dCommandId*>(&data_[write_offset_]);
		*block = command_id;

		write_offset_ += block_size;
		++command_count_;

		return reinterpret_cast<T*>(block + 1);
	}

	template<typename T>
	const T* read()
	{
		if (!is_reading_ || is_writing_)
		{
			assert(!"Invalid state.");

			return nullptr;
		}

		const auto command_size = static_cast<int>(sizeof(T));

		if ((size_ - read_offset_) < command_size)
		{
			return nullptr;
		}

		auto command = reinterpret_cast<const T*>(&data_[read_offset_]);

		read_offset_ += command_size;

		return command;
	}
}; // Renderer3dCommandBufferImpl

using Renderer3dCommandBufferPtr = Renderer3dCommandBufferImpl*;
using RendererCommandBufferUPtr = std::unique_ptr<Renderer3dCommandBufferImpl>;

//
// Renderer3dCommandBufferImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFER_INCLUDED
