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


#ifndef BSTONE_DETAIL_R3D_COMMAND_BUFFER_INCLUDED
#define BSTONE_DETAIL_R3D_COMMAND_BUFFER_INCLUDED


#include "bstone_r3d_cmd_queue.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dCmdBufferImpl
//

class R3dCmdBufferImpl final :
	public R3dCmdBuffer
{
public:
	R3dCmdBufferImpl(
		const R3dCmdQueueEnqueueParam& param);

	~R3dCmdBufferImpl() override;


	int get_command_count() const noexcept override;


	bool is_enabled() const noexcept override;

	void enable(
		const bool is_enabled) override;


	void write_begin() override;

	void write_end() override;

	R3dCmdClear* write_clear() override;

	R3dCmdViewport* write_viewport() override;

	R3dCmdScissor* write_scissor() override;
	R3dCmdScissorBox* write_scissor_box() override;

	R3dCmdCulling* write_culling() override;

	R3dCmdDepthTest* write_depth_test() override;
	R3dCmdDepthWrite* write_depth_write() override;

	R3dCmdBlending* write_blending() override;
	R3dCmdBlendingFunc* write_blending_func() override;

	R3dCmdTexture* write_texture() override;
	R3dCmdSampler* write_sampler() override;

	R3dCmdVertexInput* write_vertex_input() override;

	R3dCmdShaderStage* write_shader_stage() override;

	R3dCmdShaderVarInt32* write_shader_var_int32() override;
	R3dCmdShaderVarFloat32* write_shader_var_float32() override;
	R3dCmdShaderVarVec2* write_shader_var_vec2() override;
	R3dCmdShaderVarVec4* write_shader_var_vec4() override;
	R3dCmdShaderVarMat4* write_shader_var_mat4() override;
	R3dCmdShaderVarSampler2d* write_shader_var_sampler_2d() override;

	R3dCmdDrawIndexed* write_draw_indexed() override;


	void read_begin() override;

	void read_end() override;

	R3dCmdId read_command_id() override;

	const R3dCmdClear* read_clear() override;

	const R3dCmdViewport* read_viewport() override;

	const R3dCmdScissor* read_scissor() override;
	const R3dCmdScissorBox* read_scissor_box() override;

	const R3dCmdCulling* read_culling() override;

	const R3dCmdDepthTest* read_depth_test() override;
	const R3dCmdDepthWrite* read_depth_write() override;

	const R3dCmdBlending* read_blending() override;
	const R3dCmdBlendingFunc* read_blending_func() override;

	const R3dCmdTexture* read_texture() override;
	const R3dCmdSampler* read_sampler() override;

	const R3dCmdVertexInput* read_vertex_input() override;

	const R3dCmdShaderStage* read_shader_stage() override;

	const R3dCmdShaderVarInt32* read_shader_var_int32() override;
	const R3dCmdShaderVarFloat32* read_shader_var_float32() override;
	const R3dCmdShaderVarVec2* read_shader_var_vec2() override;
	const R3dCmdShaderVarVec4* read_shader_var_vec4() override;
	const R3dCmdShaderVarMat4* read_shader_var_mat4() override;
	const R3dCmdShaderVarSampler2d* read_shader_var_sampler_2d() override;

	const R3dCmdDrawIndexed* read_draw_indexed() override;


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
		return static_cast<int>(sizeof(R3dCmdId));
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
		const R3dCmdQueueEnqueueParam& param);

	void resize_if_necessary(
		const int dst_delta_size);

	template<typename T>
	T* write(
		const R3dCmdId command_id)
	{
		if (is_reading_ || !is_writing_)
		{
			assert(!"Invalid state.");

			return nullptr;
		}

		if (command_id == R3dCmdId::none)
		{
			assert(!"Invalid command id.");

			return nullptr;
		}

		const auto command_size = static_cast<int>(sizeof(T));

		const auto block_size = get_command_id_size() + command_size;

		resize_if_necessary(block_size);

		auto block = reinterpret_cast<R3dCmdId*>(&data_[write_offset_]);
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
}; // R3dCmdBufferImpl

using R3dCmdBufferPtr = R3dCmdBufferImpl*;
using R3dCmdBufferUPtr = std::unique_ptr<R3dCmdBufferImpl>;

//
// R3dCmdBufferImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_COMMAND_BUFFER_INCLUDED
