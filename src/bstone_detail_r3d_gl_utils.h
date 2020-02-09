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
// OpenGL 3D renderer utils (implementation).
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_DETAIL_GL_RENDERER_3D_UTILS_INCLUDED
#define BSTONE_DETAIL_GL_RENDERER_3D_UTILS_INCLUDED


#include <vector>

#include "bstone_sdl_types.h"

#include "bstone_detail_r3d_gl_api.h"
#include "bstone_detail_r3d_gl_device_features.h"
#include "bstone_detail_r3d_gl_extension_mgr.h"
#include "bstone_detail_r3d_utils.h"


namespace bstone
{
namespace detail
{


class GlRenderer3dUtils
{
public:
	static void load_library();

	static void unload_library() noexcept;


	static void* resolve_symbol(
		const char* const symbol);


	static SdlGlContextUPtr create_context(
		SdlWindowPtr sdl_window);


	static void make_context_current(
		SdlWindowPtr sdl_window,
		SdlGlContextPtr sdl_gl_context);


	static void create_window_and_context(
		const Renderer3dUtilsCreateWindowParam& param,
		SdlWindowUPtr& sdl_window,
		SdlGlContextUPtr& sdl_gl_context);


	static void window_get_drawable_size(
		SdlWindowPtr sdl_window,
		int& width,
		int& height);


	static GlContextKind context_get_kind();

	static void context_get_version(
		int& major_version,
		int& minor_version);


	static void msaa_probe(
		const Renderer3dKind renderer_kind,
		Renderer3dDeviceFeatures& device_features,
		GlDeviceFeatures& gl_device_features);

	static int msaa_window_get_value();


	static int anisotropy_get_max_value();

	static int anisotropy_clamp_value(
		const int anisotropy_value,
		const Renderer3dDeviceFeatures& device_features);

	static void anisotropy_set_value(
		const GLenum gl_target,
		const Renderer3dDeviceFeatures& device_features,
		const int anisotropy_value);

	static void anisotropy_probe(
		GlExtensionManagerPtr extension_manager,
		Renderer3dDeviceFeatures& device_features);


	static void npot_probe(
		GlExtensionManagerPtr extension_manager,
		Renderer3dDeviceFeatures& device_features);


	static void mipmap_probe(
		GlExtensionManagerPtr extension_manager,
		Renderer3dDeviceFeatures& device_features,
		GlDeviceFeatures& gl_device_features);

	static void mipmap_generate(
		const GLenum gl_target,
		const Renderer3dDeviceFeatures& device_features,
		const GlDeviceFeatures& gl_device_features);


	static void framebuffer_probe(
		GlExtensionManagerPtr extension_manager,
		GlDeviceFeatures& gl_device_features);


	static void sampler_probe(
		GlExtensionManagerPtr extension_manager,
		Renderer3dDeviceFeatures& device_features);

	static void sampler_set_anisotropy(
		const GLenum gl_sampler,
		const Renderer3dDeviceFeatures& device_features,
		const int anisotropy_value);


	static void vertex_input_vao_probe(
		GlExtensionManagerPtr extension_manager,
		GlDeviceFeatures& gl_device_features);

	static void vertex_input_probe_max_locations(
		Renderer3dDeviceFeatures& device_features);


	static void vsync_probe(
		Renderer3dDeviceFeatures& device_features);

	static bool vsync_get();

	static bool vsync_set(
		const bool is_enabled);


	static void buffer_storage_probe(
		const GlExtensionManagerPtr extension_manager,
		GlDeviceFeatures& gl_device_features);


	static void dsa_probe(
		const GlExtensionManagerPtr extension_manager,
		GlDeviceFeatures& gl_device_features);


	static void sso_probe(
		const GlExtensionManagerPtr extension_manager,
		GlDeviceFeatures& gl_device_features);


	static void swap_window(
		SdlWindowPtr sdl_window);


	static void viewport_set_rect(
		const Renderer3dViewport& viewport);

	static void viewport_set_depth_range(
		const Renderer3dViewport& viewport,
		const GlDeviceFeatures& gl_device_features);


	static void scissor_enable(
		const bool is_enabled);

	static void scissor_set_box(
		const Renderer3dScissorBox& scissor_box);


	static void culling_enable(
		const bool is_enable);

	static void culling_set_face(
		const Renderer3dCullingFace culling_face);

	static void culling_set_mode(
		const Renderer3dCullingMode culling_mode);


	static void depth_test_enable(
		const bool is_enable);

	static void depth_write_enable(
		const bool is_enable);


	static void blending_enable(
		const bool is_enable);

	static void blending_set_func(
		const Renderer3dBlendingFunc& blending_func);


	static GLenum index_buffer_get_element_type_by_byte_depth(
		const int byte_depth);

	static void renderer_features_set(
		Renderer3dDeviceFeatures& device_features);

	static Renderer3dDeviceInfo device_info_get();

	static std::string get_log(
		const bool is_shader,
		const GLuint gl_name);


	static GLenum filter_get_mag(
		const Renderer3dFilterKind mag_filter);

	static GLenum filter_get_min(
		const Renderer3dFilterKind min_filter,
		const Renderer3dMipmapMode mipmap_mode);

	static GLenum address_mode_get(
		const Renderer3dAddressMode address_mode);

	static GLenum texture_wrap_get_axis(
		const Renderer3dTextureAxis texture_axis);


private:
	static GLenum blending_get_factor(
		const Renderer3dBlendingFactor factor);

	static int msaa_window_get_max(
		const Renderer3dKind renderer_kind);

	static int msaa_fbo_get_max(
		const Renderer3dKind renderer_kind,
		Renderer3dDeviceFeatures& device_features,
		GlDeviceFeatures& gl_device_features);
}; // GlRenderer3dUtils


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_RENDERER_3D_UTILS_INCLUDED
