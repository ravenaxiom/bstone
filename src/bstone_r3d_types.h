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
// 3D renderer's types.
//


#ifndef BSTONE_R3D_TYPES_INCLUDED
#define BSTONE_R3D_TYPES_INCLUDED


#include <array>
#include <memory>
#include <string>

#include "bstone_rgb_color_model.h"


namespace bstone
{


using Renderer3dVec2 = std::array<float, 2>;
using Renderer3dVec4 = std::array<float, 4>;
using Renderer3dMat4 = std::array<float, 16>;


enum class Renderer3dKind
{
	gl_2_0,
	gl_3_2_core,

	gles_2_0,
}; // Renderer3dKind

enum class Renderer3dPrimitiveTopology
{
	none,

	point_list,

	line_list,
	line_strip,

	triangle_list,
	triangle_strip,
}; // Renderer3dPrimitiveTopology

enum class Renderer3dCullingFace
{
	counter_clockwise,
	clockwise,
}; // Renderer3dCullingFace

enum class Renderer3dCullingMode
{
	back,
	front,
	both,
}; // Renderer3dCullingMode

enum class Renderer3dBlendingFactor
{
	zero,
	one,
	src_color,
	src_alpha,
	one_minus_src_alpha,
}; // Renderer3dBlendingFactor

enum class Renderer3dAaKind
{
	none,
	ms,
}; // Renderer3dAaKind

enum class Renderer3dFilterKind
{
	nearest,
	linear,
}; // Renderer3dFilterKind

enum class Renderer3dMipmapMode
{
	none,
	nearest,
	linear,
}; // Renderer3dMipmapMode

enum class Renderer3dAddressMode
{
	clamp,
	repeat,
}; // Renderer3dAddressMode

enum class Renderer3dTextureAxis
{
	u,
	v,
}; // Renderer3dTextureAxis

enum class Renderer3dPixelFormat
{
	none,
	rgba_8_unorm,
}; // Renderer3dPixelFormat


struct Renderer3dViewport
{
	int x_;
	int y_;
	int width_;
	int height_;

	float min_depth_;
	float max_depth_;
}; // Renderer3dViewport

struct Renderer3dScissorBox
{
	int x_;
	int y_;
	int width_;
	int height_;
}; // Renderer3dScissorBox

struct Renderer3dBlendingFunc
{
	Renderer3dBlendingFactor src_factor_;
	Renderer3dBlendingFactor dst_factor_;
}; // Renderer3dBlendingFunc

struct Renderer3dClearParam
{
	Rgba8 color_;
}; // Renderer3dClearParam


// ==========================================================================
// Renderer3dDeviceFeatures
//

struct Renderer3dDeviceFeatures
{
	bool vsync_is_available_;
	bool vsync_is_requires_restart_;

	int texture_max_dimension_;

	int viewport_max_width_;
	int viewport_max_height_;

	bool anisotropy_is_available_;
	int anisotropy_max_degree_;

	bool npot_is_available_;

	bool mipmap_is_available_;

	bool sampler_is_available_;

	bool msaa_is_available_;
	bool msaa_is_render_to_window_;
	bool msaa_is_requires_restart_;
	int msaa_max_degree_;

	int vertex_input_max_locations_;
}; // Renderer3dDeviceFeatures

//
// Renderer3dDeviceFeatures
// ==========================================================================


// ==========================================================================
// Renderer3dDeviceInfo
//

struct Renderer3dDeviceInfo
{
	std::string name_;
	std::string vendor_;
	std::string version_;
}; // Renderer3dDeviceInfo

//
// Renderer3dDeviceInfo
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_TYPES_INCLUDED
