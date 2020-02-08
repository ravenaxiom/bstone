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


#include <array>
#include <memory>
#include <string>
#include <vector>

#include "bstone_rgb_color_model.h"
#include "bstone_sprite.h"


namespace bstone
{


class Renderer3dCommandQueue;
using Renderer3dCommandQueuePtr = Renderer3dCommandQueue*;


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

enum class Renderer3dBufferKind
{
	none,
	index,
	vertex,
}; // Renderer3dBufferKind

enum class Renderer3dBufferUsageKind
{
	none,

	// Target - drawing; updates - continuous.
	draw_streaming,

	// Target - drawing; updates - seldom.
	draw_static,

	// Target - drawing; updates - often.
	draw_dynamic,
}; // Renderer3dBufferUsageKind


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


// ==========================================================================
// Renderer3dBuffer
//

struct Renderer3dBufferCreateParam
{
	Renderer3dBufferKind kind_;
	Renderer3dBufferUsageKind usage_kind_;
	int size_;
}; // Renderer3dBufferCreateParam

struct Renderer3dBufferUpdateParam
{
	int offset_;
	int size_;
	const void* data_;
}; // Renderer3dBufferUpdateParam

class Renderer3dBuffer
{
protected:
	Renderer3dBuffer();


public:
	virtual ~Renderer3dBuffer();


	virtual Renderer3dBufferKind get_kind() const noexcept = 0;

	virtual Renderer3dBufferUsageKind get_usage_kind() const noexcept = 0;

	virtual int get_size() const noexcept = 0;

	virtual void update(
		const Renderer3dBufferUpdateParam& param) = 0;
}; // Renderer3dBuffer

using Renderer3dBufferPtr = Renderer3dBuffer*;
using Renderer3dBufferUPtr = std::unique_ptr<Renderer3dBuffer>;

//
// Renderer3dBuffer
// ==========================================================================


// ==========================================================================
// Renderer3dVertexInput
//

enum class Renderer3dVertexAttributeFormat
{
	none,
	rgba_8_unorm,
	rg_32_sfloat,
	rgb_32_sfloat,
}; // Renderer3dVertexAttributeFormat

struct Renderer3dVertexAttributeDescription
{
	bool is_default_;
	int location_;
	Renderer3dVertexAttributeFormat format_;
	Renderer3dBufferPtr vertex_buffer_;
	int offset_;
	int stride_;
	Renderer3dVec4 default_value_;
}; // Renderer3dVertexAttributeDescription

using Renderer3dVertexAttributeDescriptions = std::vector<Renderer3dVertexAttributeDescription>;

struct Renderer3dVertexInputCreateParam
{
	Renderer3dBufferPtr index_buffer_;
	Renderer3dVertexAttributeDescriptions attribute_descriptions_;
}; // Renderer3dVertexInputCreateParam


class Renderer3dVertexInput
{
protected:
	Renderer3dVertexInput() = default;


public:
	virtual ~Renderer3dVertexInput() = default;
}; // Renderer3dVertexInput

using Renderer3dVertexInputPtr = Renderer3dVertexInput*;
using Renderer3dVertexInputUPtr = std::unique_ptr<Renderer3dVertexInput>;

//
// Renderer3dVertexInput
// ==========================================================================


// ==========================================================================
// Renderer3dTexture2d
//

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

struct Renderer3dSamplerState
{
	Renderer3dFilterKind mag_filter_;
	Renderer3dFilterKind min_filter_;

	Renderer3dMipmapMode mipmap_mode_;

	Renderer3dAddressMode address_mode_u_;
	Renderer3dAddressMode address_mode_v_;

	int anisotropy_;
}; // Renderer3dSamplerState

enum class Renderer3dPixelFormat
{
	none,
	rgba_8_unorm,
}; // Renderer3dPixelFormat

struct Renderer3dTexture2dCreateParam
{
	Renderer3dPixelFormat pixel_format_;

	int width_;
	int height_;

	int mipmap_count_;
}; // Renderer3dTexture2dCreateParam

struct Renderer3dTexture2dUpdateParam
{
	int mipmap_level_;

	const void* image_;
}; // Renderer3dTexture2dUpdateParam

class Renderer3dTexture2d
{
protected:
	Renderer3dTexture2d() = default;


public:
	virtual ~Renderer3dTexture2d() = default;


	virtual void update(
		const Renderer3dTexture2dUpdateParam& param) = 0;

	virtual void generate_mipmaps() = 0;
}; // Renderer3dTexture2d

using Renderer3dTexture2dPtr = Renderer3dTexture2d*;
using Renderer3dTexture2dUPtr = std::unique_ptr<Renderer3dTexture2d>;


struct Renderer3dSamplerCreateParam
{
	Renderer3dSamplerState state_;
}; // Renderer3dSamplerCreateParam

struct Renderer3dSamplerUpdateParam
{
	Renderer3dSamplerState state_;
}; // Renderer3dSamplerUpdateParam

class Renderer3dSampler
{
protected:
	Renderer3dSampler() = default;


public:
	virtual ~Renderer3dSampler() = default;


	virtual void update(
		const Renderer3dSamplerUpdateParam& param) = 0;

	virtual const Renderer3dSamplerState& get_state() const noexcept = 0;
}; // Renderer3dSampler

using Renderer3dSamplerPtr = Renderer3dSampler*;
using Renderer3dSamplerUPtr = std::unique_ptr<Renderer3dSampler>;

//
// Renderer3dTexture2d
// ==========================================================================


// ==========================================================================
// Shader
//

enum class Renderer3dShaderVarKind
{
	none,
	attribute,
	sampler,
	uniform,
}; // Renderer3dShaderVarKind

enum class Renderer3dShaderVarTypeId
{
	none,
	int32,
	float32,
	vec2,
	vec3,
	vec4,
	mat4,
	sampler2d,
}; // Renderer3dShaderVarTypeId

class Renderer3dShaderVar
{
protected:
	Renderer3dShaderVar();

	virtual ~Renderer3dShaderVar();


public:
	virtual Renderer3dShaderVarKind get_kind() const noexcept = 0;

	virtual Renderer3dShaderVarTypeId get_type_id() const noexcept = 0;

	virtual int get_index() const noexcept = 0;

	virtual const std::string& get_name() const noexcept = 0;

	virtual int get_input_index() const noexcept = 0;
}; // Renderer3dShaderVar

using Renderer3dShaderVarPtr = Renderer3dShaderVar*;
using Renderer3dShaderVarCPtr = const Renderer3dShaderVar*;


class Renderer3dShaderVarInt32 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarInt32();

	virtual ~Renderer3dShaderVarInt32();


public:
	virtual void set_int(
		const std::int32_t value) = 0;
}; // Renderer3dShaderVarInt32

using Renderer3dShaderVarInt32Ptr = Renderer3dShaderVarInt32*;
using Renderer3dShaderVarInt32CPtr = const Renderer3dShaderVarInt32*;


class Renderer3dShaderVarFloat32 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarFloat32();

	virtual ~Renderer3dShaderVarFloat32();


public:
	virtual void set_float(
		const float value) = 0;
}; // Renderer3dShaderVarFloat32

using Renderer3dShaderVarFloat32Ptr = Renderer3dShaderVarFloat32*;
using Renderer3dShaderVarFloat32CPtr = const Renderer3dShaderVarFloat32*;


class Renderer3dShaderVarVec2 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarVec2();

	virtual ~Renderer3dShaderVarVec2();


public:
	virtual void set_vec2(
		const float* const value) = 0;
}; // Renderer3dShaderVarVec2

using Renderer3dShaderVarVec2Ptr = Renderer3dShaderVarVec2*;
using Renderer3dShaderVarVec2CPtr = const Renderer3dShaderVarVec2*;


class Renderer3dShaderVarVec4 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarVec4();

	virtual ~Renderer3dShaderVarVec4();


public:
	virtual void set_vec4(
		const float* const value) = 0;
}; // Renderer3dShaderVarVec4

using Renderer3dShaderVarVec4Ptr = Renderer3dShaderVarVec4*;
using Renderer3dShaderVarVec4CPtr = const Renderer3dShaderVarVec4*;


class Renderer3dShaderVarMat4 :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarMat4();

	virtual ~Renderer3dShaderVarMat4();


public:
	virtual void set_mat4(
		const float* const value) = 0;
}; // Renderer3dShaderVarMat4

using Renderer3dShaderVarMat4Ptr = Renderer3dShaderVarMat4*;
using Renderer3dShaderVarMat4CPtr = const Renderer3dShaderVarMat4*;


class Renderer3dShaderVarSampler2d :
	public virtual Renderer3dShaderVar
{
protected:
	Renderer3dShaderVarSampler2d();

	virtual ~Renderer3dShaderVarSampler2d();


public:
	virtual void set_sampler_2d(
		const std::int32_t value) = 0;
}; // Renderer3dShaderVarSampler2d

using Renderer3dShaderVarSampler2dPtr = Renderer3dShaderVarSampler2d*;
using Renderer3dShaderVarSampler2dCPtr = const Renderer3dShaderVarSampler2d*;


enum class Renderer3dShaderKind
{
	none,
	fragment,
	vertex,
}; // Renderer3dShaderKind

struct Renderer3dShaderSource
{
	const void* data_;
	int size_;
}; // Source

struct Renderer3dShaderCreateParam
{
	Renderer3dShaderKind kind_;
	Renderer3dShaderSource source_;
}; // CreateParam

class Renderer3dShader
{
protected:
	Renderer3dShader();


public:
	virtual ~Renderer3dShader();


	virtual Renderer3dShaderKind get_kind() const noexcept = 0;
}; // Renderer3dShader

using Renderer3dShaderPtr = Renderer3dShader*;
using Renderer3dShaderUPtr = std::unique_ptr<Renderer3dShader>;


struct Renderer3dShaderStageInputBinding
{
	int index_;
	std::string name_;
}; // Renderer3dShaderStageInputBinding

using Renderer3dShaderStageInputBindings = std::vector<Renderer3dShaderStageInputBinding>;


struct Renderer3dShaderStageCreateParam
{
	Renderer3dShaderPtr fragment_shader_;
	Renderer3dShaderPtr vertex_shader_;
	Renderer3dShaderStageInputBindings input_bindings_;
}; // Renderer3dShaderStageCreateParam

class Renderer3dShaderStage
{
protected:
	Renderer3dShaderStage();


public:
	virtual ~Renderer3dShaderStage();


	virtual void set() = 0;


	virtual Renderer3dShaderVarPtr find_var(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarInt32Ptr find_var_int32(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarFloat32Ptr find_var_float32(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarVec2Ptr find_var_vec2(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarVec4Ptr find_var_vec4(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarMat4Ptr find_var_mat4(
		const std::string& name) = 0;

	virtual Renderer3dShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) = 0;
}; // Renderer3dShaderStage

using Renderer3dShaderStagePtr = Renderer3dShaderStage*;
using Renderer3dShaderStageUPtr = std::unique_ptr<Renderer3dShaderStage>;

//
// Shader
// ==========================================================================


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
