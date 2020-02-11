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


#ifndef BSTONE_R3D_INCLUDED
#define BSTONE_R3D_INCLUDED


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


class R3dCmdQueue;
using R3dCmdQueuePtr = R3dCmdQueue*;


struct R3dCreateWindowParam
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
}; // R3dCreateWindowParam

struct R3dWindowSetModeParam
{
	bool is_windowed_;
	bool is_positioned_;

	int x_;
	int y_;

	int width_;
	int height_;
}; // R3dWindowSetModeParam

struct R3dCreateParam
{
	R3dKind renderer_kind_;
	R3dCreateWindowParam window_;

	R3dAaKind aa_kind_;
	int aa_value_;

	bool is_vsync_;
}; // R3dCreateParam

struct R3dDrawIndexedParam
{
	// Primitive topology.
	R3dPrimitiveTopology primitive_topology_;

	// Vertex count.
	int vertex_count_;

	// Size of the element in bytes.
	int index_byte_depth_;

	// Offset in bytes from the start of index buffer.
	int index_buffer_offset_;

	// Number of the index to draw from.
	int index_offset_;
}; // R3dDrawIndexedParam


class R3d
{
protected:
	R3d() = default;

	virtual ~R3d() = default;


public:
	virtual R3dKind get_kind() const noexcept = 0;

	virtual const std::string& get_name() const noexcept = 0;

	virtual const std::string& get_description() const noexcept = 0;


	virtual const R3dDeviceFeatures& device_get_features() const noexcept = 0;

	virtual const R3dDeviceInfo& device_get_info() const noexcept = 0;


	virtual void window_set_mode(
		const R3dWindowSetModeParam& param) = 0;

	virtual void window_set_title(
		const std::string& title_utf8) = 0;

	virtual void window_show(
		const bool is_visible) = 0;


	virtual bool vsync_get() const noexcept = 0;

	virtual void vsync_set(
		const bool is_enabled) = 0;


	virtual void aa_set(
		const R3dAaKind aa_kind,
		const int aa_value) = 0;


	virtual void present() = 0;


	virtual R3dBufferUPtr buffer_create(
		const R3dBufferCreateParam& param) = 0;

	virtual R3dTexture2dUPtr texture_2d_create(
		const R3dTexture2dCreateParam& param) = 0;

	virtual R3dSamplerUPtr sampler_create(
		const R3dSamplerCreateParam& param) = 0;

	virtual R3dVertexInputUPtr vertex_input_create(
		const R3dVertexInputCreateParam& param) = 0;

	virtual R3dShaderUPtr shader_create(
		const R3dShaderCreateParam& param) = 0;

	virtual R3dShaderStageUPtr shader_stage_create(
		const R3dShaderStageCreateParam& param) = 0;


	virtual void submit_commands(
		const R3dCmdQueuePtr command_queue) = 0;
}; // R3d

using R3dPtr = R3d*;


} // bstone


#endif // !BSTONE_R3D_INCLUDED
