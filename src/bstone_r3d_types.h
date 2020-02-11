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


using R3dVec2 = std::array<float, 2>;
using R3dVec4 = std::array<float, 4>;
using R3dMat4 = std::array<float, 16>;


enum class R3dKind
{
	gl_2_0,
	gl_3_2_core,

	gles_2_0,
}; // R3dKind

enum class R3dPrimitiveTopology
{
	none,

	point_list,

	line_list,
	line_strip,

	triangle_list,
	triangle_strip,
}; // R3dPrimitiveTopology

enum class R3dCullingFace
{
	counter_clockwise,
	clockwise,
}; // R3dCullingFace

enum class R3dCullingMode
{
	back,
	front,
	both,
}; // R3dCullingMode

enum class R3dBlendingFactor
{
	zero,
	one,
	src_color,
	src_alpha,
	one_minus_src_alpha,
}; // R3dBlendingFactor

enum class R3dAaKind
{
	none,
	ms,
}; // R3dAaKind

enum class R3dFilterKind
{
	nearest,
	linear,
}; // R3dFilterKind

enum class R3dMipmapMode
{
	none,
	nearest,
	linear,
}; // R3dMipmapMode

enum class R3dAddressMode
{
	clamp,
	repeat,
}; // R3dAddressMode

enum class R3dTextureAxis
{
	u,
	v,
}; // R3dTextureAxis

enum class R3dPixelFormat
{
	none,
	rgba_8_unorm,
}; // R3dPixelFormat


struct R3dViewport
{
	int x_;
	int y_;
	int width_;
	int height_;

	float min_depth_;
	float max_depth_;
}; // R3dViewport

struct R3dScissorBox
{
	int x_;
	int y_;
	int width_;
	int height_;
}; // R3dScissorBox

struct R3dBlendingFunc
{
	R3dBlendingFactor src_factor_;
	R3dBlendingFactor dst_factor_;
}; // R3dBlendingFunc

struct R3dClearParam
{
	Rgba8 color_;
}; // R3dClearParam


// ==========================================================================
// R3dDeviceFeatures
//

struct R3dDeviceFeatures
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
}; // R3dDeviceFeatures

//
// R3dDeviceFeatures
// ==========================================================================


// ==========================================================================
// R3dDeviceInfo
//

struct R3dDeviceInfo
{
	std::string name_;
	std::string vendor_;
	std::string version_;
}; // R3dDeviceInfo

//
// R3dDeviceInfo
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_TYPES_INCLUDED
