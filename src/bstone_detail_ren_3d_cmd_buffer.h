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


	int get_count() const noexcept override;


	bool is_enabled() const noexcept override;

	void enable(
		const bool is_enabled) override;


	void begin_write() override;

	void end_write() override;

	Ren3dClearCmd* write_clear() override;

	Ren3dSetViewportCmd* write_set_viewport() override;

	Ren3dEnableScissorCmd* write_enable_scissor() override;
	Ren3dSetScissorBoxCmd* write_set_scissor_box() override;

	Ren3dEnableCullingCmd* write_enable_culling() override;

	Ren3dEnableDepthTestCmd* write_enable_depth_test() override;
	Ren3dEnableDepthWriteCmd* write_enable_depth_write() override;

	Ren3dEnableBlendingCmd* write_enable_blending() override;
	Ren3dSetBlendingFuncCmd* write_set_blending_func() override;

	Ren3dSetTextureCmd* write_set_texture() override;
	Ren3dSetSamplerCmd* write_set_sampler() override;

	Ren3dSetVertexInputCmd* write_set_vertex_input() override;

	Ren3dSetShaderStageCmd* write_set_shader_stage() override;

	Ren3dSetInt32UniformCmd* write_set_int32_uniform() override;
	Ren3dSetFloat32UniformCmd* write_set_float32_uniform() override;
	Ren3dSetVec2UniformCmd* write_set_vec2_uniform() override;
	Ren3dSetVec4UniformCmd* write_set_vec4_uniform() override;
	Ren3dSetMat4UniformCmd* write_set_mat4_uniform() override;
	Ren3dSetSampler2dUniformCmd* write_set_sampler_2d_uniform() override;

	Ren3dDrawIndexedCmd* write_draw_indexed() override;


	void begin_read() override;

	void end_read() override;

	Ren3dCmdId read_command_id() override;

	const Ren3dClearCmd* read_clear() override;

	const Ren3dSetViewportCmd* read_set_viewport() override;

	const Ren3dEnableScissorCmd* read_enable_scissor() override;
	const Ren3dSetScissorBoxCmd* read_set_scissor_box() override;

	const Ren3dEnableCullingCmd* read_enable_culling() override;

	const Ren3dEnableDepthTestCmd* read_enable_depth_test() override;
	const Ren3dEnableDepthWriteCmd* read_enable_depth_write() override;

	const Ren3dEnableBlendingCmd* read_enable_blending() override;
	const Ren3dSetBlendingFuncCmd* read_set_blending_func() override;

	const Ren3dSetTextureCmd* read_set_texture() override;
	const Ren3dSetSamplerCmd* read_set_sampler() override;

	const Ren3dSetVertexInputCmd* read_set_vertex_input() override;

	const Ren3dSetShaderStageCmd* read_set_shader_stage() override;

	const Ren3dSetInt32UniformCmd* read_set_int32_uniform() override;
	const Ren3dSetFloat32UniformCmd* read_set_float32_uniform() override;
	const Ren3dSetVec2UniformCmd* read_set_vec2_uniform() override;
	const Ren3dSetVec4UniformCmd* read_set_vec4_uniform() override;
	const Ren3dSetMat4UniformCmd* read_set_mat4_uniform() override;
	const Ren3dSetSampler2dUniformCmd* read_set_sampler_2d_uniform() override;

	const Ren3dDrawIndexedCmd* read_draw_indexed() override;


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
