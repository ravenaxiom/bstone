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
// 3D renderer.
//


#ifndef BSTONE_RENDERER_3D_INCLUDED
#define BSTONE_RENDERER_3D_INCLUDED


#include <memory>
#include <string>

#include "bstone_r3d_buffer.h"
#include "bstone_r3d_texture_2d.h"
#include "bstone_r3d_types.h"
#include "bstone_r3d_sampler.h"
#include "bstone_r3d_shader.h"
#include "bstone_r3d_shader_stage.h"
#include "bstone_r3d_vertex_input.h"


namespace bstone
{


class Renderer3dCommandQueue;
using Renderer3dCommandQueuePtr = Renderer3dCommandQueue*;


struct Renderer3dCreateWindowParam
{
	bool is_visible_;
	bool is_borderless_;
	bool is_fullscreen_desktop_;
	bool is_positioned_;

	int x_;
	int y_;

	int width_;
	int height_;

	std::string title_utf8_;
}; // Renderer3dCreateWindowParam

struct Renderer3dWindowSetModeParam
{
	bool is_windowed_;
	bool is_positioned_;

	int x_;
	int y_;

	int width_;
	int height_;
}; // Renderer3dWindowSetModeParam

struct Renderer3dCreateParam
{
	Renderer3dKind renderer_kind_;
	Renderer3dCreateWindowParam window_;

	Renderer3dAaKind aa_kind_;
	int aa_value_;

	bool is_vsync_;
}; // Renderer3dCreateParam

struct Renderer3dDrawIndexedParam
{
	// Primitive topology.
	Renderer3dPrimitiveTopology primitive_topology_;

	// Vertex count.
	int vertex_count_;

	// Size of the element in bytes.
	int index_byte_depth_;

	// Offset in bytes from the start of index buffer.
	int index_buffer_offset_;

	// Number of the index to draw from.
	int index_offset_;
}; // Renderer3dDrawIndexedParam


class Renderer3d
{
protected:
	Renderer3d() = default;

	virtual ~Renderer3d() = default;


public:
	virtual Renderer3dKind get_kind() const noexcept = 0;

	virtual const std::string& get_name() const noexcept = 0;

	virtual const std::string& get_description() const noexcept = 0;


	virtual const Renderer3dDeviceFeatures& device_get_features() const noexcept = 0;

	virtual const Renderer3dDeviceInfo& device_get_info() const noexcept = 0;


	virtual void window_set_mode(
		const Renderer3dWindowSetModeParam& param) = 0;

	virtual void window_set_title(
		const std::string& title_utf8) = 0;

	virtual void window_show(
		const bool is_visible) = 0;


	virtual bool vsync_get() const noexcept = 0;

	virtual void vsync_set(
		const bool is_enabled) = 0;


	virtual void aa_set(
		const Renderer3dAaKind aa_kind,
		const int aa_value) = 0;


	virtual void present() = 0;


	virtual Renderer3dBufferUPtr buffer_create(
		const Renderer3dBufferCreateParam& param) = 0;

	virtual Renderer3dTexture2dUPtr texture_2d_create(
		const Renderer3dTexture2dCreateParam& param) = 0;

	virtual Renderer3dSamplerUPtr sampler_create(
		const Renderer3dSamplerCreateParam& param) = 0;

	virtual Renderer3dVertexInputUPtr vertex_input_create(
		const Renderer3dVertexInputCreateParam& param) = 0;

	virtual Renderer3dShaderUPtr shader_create(
		const Renderer3dShaderCreateParam& param) = 0;

	virtual Renderer3dShaderStageUPtr shader_stage_create(
		const Renderer3dShaderStageCreateParam& param) = 0;


	virtual void submit_commands(
		const Renderer3dCommandQueuePtr command_queue) = 0;
}; // Renderer3d

using Renderer3dPtr = Renderer3d*;


} // bstone


#endif // !BSTONE_RENDERER_3D_INCLUDED
