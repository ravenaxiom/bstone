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


#ifndef BSTONE_DETAIL_REN_3D_COMMAND_BUFFER_INCLUDED
#define BSTONE_DETAIL_REN_3D_COMMAND_BUFFER_INCLUDED


#include "bstone_ren_3d_cmd_queue.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dCmdBufferImpl
//

class Ren3dCmdBufferImpl final :
	public Ren3dCmdBuffer
{
public:
	Ren3dCmdBufferImpl(
		const Ren3dCmdQueueEnqueueParam& param);

	~Ren3dCmdBufferImpl() override;


	int get_command_count() const noexcept override;


	bool is_enabled() const noexcept override;

	void enable(
		const bool is_enabled) override;


	void write_begin() override;

	void write_end() override;

	Ren3dCmdClear* write_clear() override;

	Ren3dCmdViewport* write_viewport() override;

	Ren3dCmdScissor* write_scissor() override;
	Ren3dCmdScissorBox* write_scissor_box() override;

	Ren3dCmdCulling* write_culling() override;

	Ren3dCmdDepthTest* write_depth_test() override;
	Ren3dCmdDepthWrite* write_depth_write() override;

	Ren3dCmdBlending* write_blending() override;
	Ren3dCmdBlendingFunc* write_blending_func() override;

	Ren3dCmdTexture* write_texture() override;
	Ren3dCmdSampler* write_sampler() override;

	Ren3dCmdVertexInput* write_vertex_input() override;

	Ren3dCmdShaderStage* write_shader_stage() override;

	Ren3dCmdShaderVarInt32* write_shader_var_int32() override;
	Ren3dCmdShaderVarFloat32* write_shader_var_float32() override;
	Ren3dCmdShaderVarVec2* write_shader_var_vec2() override;
	Ren3dCmdShaderVarVec4* write_shader_var_vec4() override;
	Ren3dCmdShaderVarMat4* write_shader_var_mat4() override;
	Ren3dCmdShaderVarSampler2d* write_shader_var_sampler_2d() override;

	Ren3dCmdDrawIndexed* write_draw_indexed() override;


	void read_begin() override;

	void read_end() override;

	Ren3dCmdId read_command_id() override;

	const Ren3dCmdClear* read_clear() override;

	const Ren3dCmdViewport* read_viewport() override;

	const Ren3dCmdScissor* read_scissor() override;
	const Ren3dCmdScissorBox* read_scissor_box() override;

	const Ren3dCmdCulling* read_culling() override;

	const Ren3dCmdDepthTest* read_depth_test() override;
	const Ren3dCmdDepthWrite* read_depth_write() override;

	const Ren3dCmdBlending* read_blending() override;
	const Ren3dCmdBlendingFunc* read_blending_func() override;

	const Ren3dCmdTexture* read_texture() override;
	const Ren3dCmdSampler* read_sampler() override;

	const Ren3dCmdVertexInput* read_vertex_input() override;

	const Ren3dCmdShaderStage* read_shader_stage() override;

	const Ren3dCmdShaderVarInt32* read_shader_var_int32() override;
	const Ren3dCmdShaderVarFloat32* read_shader_var_float32() override;
	const Ren3dCmdShaderVarVec2* read_shader_var_vec2() override;
	const Ren3dCmdShaderVarVec4* read_shader_var_vec4() override;
	const Ren3dCmdShaderVarMat4* read_shader_var_mat4() override;
	const Ren3dCmdShaderVarSampler2d* read_shader_var_sampler_2d() override;

	const Ren3dCmdDrawIndexed* read_draw_indexed() override;


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
		return static_cast<int>(sizeof(Ren3dCmdId));
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


	void validate_param(
		const Ren3dCmdQueueEnqueueParam& param);

	void resize_if_necessary(
		const int dst_delta_size);

	template<typename T>
	T* write(
		const Ren3dCmdId command_id)
	{
		if (is_reading_ || !is_writing_)
		{
			assert(!"Invalid state.");

			return nullptr;
		}

		if (command_id == Ren3dCmdId::none)
		{
			assert(!"Invalid command id.");

			return nullptr;
		}

		const auto command_size = static_cast<int>(sizeof(T));

		const auto block_size = get_command_id_size() + command_size;

		resize_if_necessary(block_size);

		auto block = reinterpret_cast<Ren3dCmdId*>(&data_[write_offset_]);
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
}; // Ren3dCmdBufferImpl

using Ren3dCmdBufferPtr = Ren3dCmdBufferImpl*;
using Ren3dCmdBufferUPtr = std::unique_ptr<Ren3dCmdBufferImpl>;

//
// Ren3dCmdBufferImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_COMMAND_BUFFER_INCLUDED
