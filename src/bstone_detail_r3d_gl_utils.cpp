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
// OpenGL 3D renderer utils.
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_utils.h"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <limits>
#include <sstream>

#include "SDL_video.h"

#include "bstone_exception.h"
#include "bstone_r3d_limits.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_error.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlRenderer3dUtils
//

void GlRenderer3dUtils::load_library()
{
	const auto error_message_prefix = "Failed to load default OpenGL library. ";

	const auto sdl_result = ::SDL_GL_LoadLibrary(nullptr);

	if (sdl_result < 0)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}
}

void GlRenderer3dUtils::unload_library() noexcept
{
	::SDL_GL_UnloadLibrary();
}

void* GlRenderer3dUtils::resolve_symbol(
	const char* const symbol)
{
	return ::SDL_GL_GetProcAddress(symbol);
}

SdlGlContextUPtr GlRenderer3dUtils::create_context(
	SdlWindowPtr sdl_window)
{
	const auto error_message_prefix = "Failed to create OpenGL context. ";

	if (!sdl_window)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += "Null SDL window.";

		throw Exception{error_message};
	}

	auto sdl_gl_context = SdlGlContextUPtr{::SDL_GL_CreateContext(sdl_window)};

	if (!sdl_gl_context)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}

	return sdl_gl_context;
}

void GlRenderer3dUtils::make_context_current(
	SdlWindowPtr sdl_window,
	SdlGlContextPtr sdl_gl_context)
{
	const auto error_message_prefix = "Failed to make a context current. ";

	if (!sdl_window)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += "Null SDL window.";

		throw Exception{error_message};
	}

	const auto sdl_result = ::SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);

	if (sdl_result < 0)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}
}

void GlRenderer3dUtils::create_window_and_context(
	const Renderer3dUtilsCreateWindowParam& param,
	SdlWindowUPtr& sdl_window,
	SdlGlContextUPtr& sdl_gl_context)
{
	auto sdl_window_result = Renderer3dUtils::window_create(param);
	auto sdl_gl_context_result = create_context(sdl_window_result.get());

	sdl_window = std::move(sdl_window_result);
	sdl_gl_context = std::move(sdl_gl_context_result);
}

void GlRenderer3dUtils::window_get_drawable_size(
	SdlWindowPtr sdl_window,
	int& width,
	int& height)
{
	width = 0;
	height = 0;

	if (!sdl_window)
	{
		throw Exception{"Null window."};
	}

	::SDL_GL_GetDrawableSize(sdl_window, &width, &height);

	if (width <= 0 || height <= 0)
	{
		throw Exception{::SDL_GetError()};
	}
}

GlContextKind GlRenderer3dUtils::context_get_kind()
{
	const auto sdl_gl_context = ::SDL_GL_GetCurrentContext();

	if (sdl_gl_context == nullptr)
	{
		return GlContextKind::invalid;
	}

	auto sdl_attribute = 0;

	const auto sdl_result = ::SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &sdl_attribute);

	if (sdl_result != 0)
	{
		return GlContextKind::invalid;
	}

	switch (sdl_attribute)
	{
		case 0:
			return GlContextKind::none;

		case SDL_GL_CONTEXT_PROFILE_CORE:
			return GlContextKind::core;

		case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
			return GlContextKind::compatibility;

		case SDL_GL_CONTEXT_PROFILE_ES:
			return GlContextKind::es;

		default:
			return GlContextKind::invalid;
	}
}

void GlRenderer3dUtils::context_get_version(
	int& major_version,
	int& minor_version)
{
	major_version = 0;
	minor_version = 0;

	const auto sdl_gl_context = ::SDL_GL_GetCurrentContext();

	if (sdl_gl_context == nullptr)
	{
		throw Exception{"No current context."};
	}

	auto sdl_result = 0;

	// Major.
	//
	auto sdl_major_version = 0;

	sdl_result = ::SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &sdl_major_version);

	if (sdl_result != 0)
	{
		auto error_message = std::string{"Failed to get major version. "};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}

	// Minor.
	//
	auto sdl_minor_version = 0;

	sdl_result = ::SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &sdl_minor_version);

	if (sdl_result != 0)
	{
		auto error_message = std::string{"Failed to get minor version. "};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}

	// Result.
	//
	major_version = sdl_major_version;
	minor_version = sdl_minor_version;
}

int GlRenderer3dUtils::anisotropy_clamp_value(
	const int anisotropy_value,
	const Renderer3dDeviceFeatures& device_features)
{
	auto clamped_value = anisotropy_value;

	if (clamped_value < Renderer3dLimits::anisotropy_min_off)
	{
		clamped_value = Renderer3dLimits::anisotropy_min_off;
	}
	else if (clamped_value < Renderer3dLimits::anisotropy_min_on)
	{
		clamped_value = Renderer3dLimits::anisotropy_min_on;
	}
	else if (clamped_value > device_features.anisotropy_max_degree_)
	{
		clamped_value = device_features.anisotropy_max_degree_;
	}

	return clamped_value;
}

int GlRenderer3dUtils::msaa_window_get_max(
	const Renderer3dKind renderer_kind)
{
	const auto sdl_gl_current_context = SDL_GL_GetCurrentContext();

	if (sdl_gl_current_context)
	{
		throw Exception{"OpenGL context already exists."};
	}

	try
	{
		auto window_param = Renderer3dUtilsCreateWindowParam{};
		window_param.renderer_kind_ = renderer_kind;
		window_param.window_.is_borderless_ = true;
		window_param.window_.width_ = 1;
		window_param.window_.height_ = 1;
		window_param.aa_kind_ = Renderer3dAaKind::ms;

		auto max_msaa = Renderer3dLimits::aa_min_off;

		for (int i = Renderer3dLimits::aa_min_on; i <= Renderer3dLimits::aa_max; i *= 2)
		{
			window_param.aa_value_ = i;

			auto sdl_window = SdlWindowUPtr{};
			auto sdl_gl_context = SdlGlContextUPtr{};

			create_window_and_context(window_param, sdl_window, sdl_gl_context);

			auto sdl_sample_count = 0;

			const auto sdl_result = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &sdl_sample_count);

			if (sdl_result != 0)
			{
				break;
			}

			if (sdl_sample_count > 0)
			{
				max_msaa = std::max(i, max_msaa);
			}
			else
			{
				break;
			}
		}

		return max_msaa;
	}
	catch (const Exception&)
	{
		return Renderer3dLimits::aa_min_off;
	}
}

int GlRenderer3dUtils::msaa_fbo_get_max(
	const Renderer3dKind renderer_kind,
	Renderer3dDeviceFeatures& device_features,
	GlDeviceFeatures& gl_device_features)
{
	const auto sdl_gl_current_context = SDL_GL_GetCurrentContext();

	if (sdl_gl_current_context)
	{
		throw Exception{"OpenGL context already exists."};
	}

	try
	{
		auto window_param = Renderer3dUtilsCreateWindowParam{};
		window_param.renderer_kind_ = renderer_kind;
		window_param.window_.is_borderless_ = true;
		window_param.window_.width_ = 1;
		window_param.window_.height_ = 1;

		auto sdl_window = SdlWindowUPtr{};
		auto sdl_gl_context = SdlGlContextUPtr{};

		create_window_and_context(window_param, sdl_window, sdl_gl_context);

		auto extension_manager = detail::GlExtensionManagerFactory::create();

		extension_manager->probe(GlExtensionId::essentials);

		if (!extension_manager->has(GlExtensionId::essentials))
		{
			throw Exception{"Essential functions not available."};
		}

		GlRenderer3dUtils::framebuffer_probe(
			extension_manager.get(),
			gl_device_features
		);

		if (!gl_device_features.framebuffer_is_available_)
		{
			return Renderer3dLimits::aa_min_off;
		}


		const auto gl_enum = (gl_device_features.framebuffer_is_ext_ ? GL_MAX_SAMPLES_EXT : GL_MAX_SAMPLES);

		auto gl_value = GLint{};

		glGetIntegerv(gl_enum, &gl_value);
		GlError::ensure_debug();

		return gl_value;
	}
	catch (const Exception&)
	{
		return Renderer3dLimits::aa_min_off;
	}
}

void GlRenderer3dUtils::msaa_probe(
	const Renderer3dKind renderer_kind,
	Renderer3dDeviceFeatures& device_features,
	GlDeviceFeatures& gl_device_features)
{
	device_features.msaa_is_available_ = false;
	device_features.msaa_is_render_to_window_ = false;
	device_features.msaa_is_requires_restart_ = false;
	device_features.msaa_max_degree_ = Renderer3dLimits::aa_min_off;

	const auto msaa_window_max = GlRenderer3dUtils::msaa_window_get_max(
		renderer_kind);

	if (msaa_window_max >= Renderer3dLimits::aa_min_on)
	{
		device_features.msaa_is_available_ = true;

		if (msaa_window_max > device_features.msaa_max_degree_)
		{
			device_features.msaa_max_degree_ = msaa_window_max;
		}
	}

	const auto msaa_fbo_max = GlRenderer3dUtils::msaa_fbo_get_max(
		renderer_kind,
		device_features,
		gl_device_features
	);

	if (msaa_fbo_max >= Renderer3dLimits::aa_min_on)
	{
		device_features.msaa_is_available_ = true;

		if (msaa_fbo_max > device_features.msaa_max_degree_)
		{
			device_features.msaa_max_degree_ = msaa_fbo_max;
		}
	}

	if (msaa_window_max >= Renderer3dLimits::aa_min_on &&
		msaa_fbo_max < Renderer3dLimits::aa_min_on)
	{
		device_features.msaa_is_render_to_window_ = true;
		device_features.msaa_is_requires_restart_ = true;
	}
}

int GlRenderer3dUtils::msaa_window_get_value()
{
	{
		auto sdl_buffer_count = 0;

		const auto sdl_result = ::SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &sdl_buffer_count);

		if (sdl_result != 0)
		{
			auto message = std::string{"Failed to get multisample buffer count. "};
			message += ::SDL_GetError();

			throw Exception{message};
		}

		if (sdl_buffer_count <= 0)
		{
			return 0;
		}
	}

	{
		auto sdl_sample_count = 0;

		const auto sdl_result = ::SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &sdl_sample_count);

		if (sdl_result != 0)
		{
			auto message = std::string{"Failed to get multisample sample count. "};
			message += ::SDL_GetError();

			throw Exception{message};
		}

		return sdl_sample_count;
	}
}

int GlRenderer3dUtils::anisotropy_get_max_value()
{
	auto gl_max_value = GLfloat{};

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &gl_max_value);

	GlError::ensure_debug();

	if (gl_max_value <= static_cast<GLfloat>(Renderer3dLimits::anisotropy_min_off))
	{
		return Renderer3dLimits::anisotropy_min_off;
	}

	return static_cast<int>(gl_max_value);
}

void GlRenderer3dUtils::anisotropy_set_value(
	const GLenum gl_target,
	const Renderer3dDeviceFeatures& device_features,
	const int anisotropy_value)
{
	if (!device_features.anisotropy_is_available_)
	{
		return;
	}

	const auto clamped_value = anisotropy_clamp_value(anisotropy_value, device_features);

	const auto gl_value = static_cast<GLfloat>(clamped_value);

	glTexParameterf(gl_target, GL_TEXTURE_MAX_ANISOTROPY, gl_value);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::anisotropy_probe(
	GlExtensionManagerPtr extension_manager,
	Renderer3dDeviceFeatures& device_features)
{
	device_features.anisotropy_is_available_ = false;
	device_features.anisotropy_max_degree_ = Renderer3dLimits::anisotropy_min_off;

#ifndef BSTONE_RENDERER_3D_TEST_NO_ANISOTROPY
	if (!device_features.anisotropy_is_available_)
	{
		extension_manager->probe(GlExtensionId::arb_texture_filter_anisotropic);

		device_features.anisotropy_is_available_ =
			extension_manager->has(GlExtensionId::arb_texture_filter_anisotropic);
	}

	if (!device_features.anisotropy_is_available_)
	{
		extension_manager->probe(GlExtensionId::ext_texture_filter_anisotropic);

		device_features.anisotropy_is_available_ =
			extension_manager->has(GlExtensionId::ext_texture_filter_anisotropic);
	}

	if (device_features.anisotropy_is_available_)
	{
		device_features.anisotropy_max_degree_ = anisotropy_get_max_value();
	}
#endif // !BSTONE_RENDERER_3D_TEST_NO_ANISOTROPY
}

void GlRenderer3dUtils::npot_probe(
	GlExtensionManagerPtr extension_manager,
	Renderer3dDeviceFeatures& device_features)
{
	device_features.npot_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_POT_ONLY
	if (!device_features.npot_is_available_)
	{
		extension_manager->probe(GlExtensionId::arb_texture_non_power_of_two);

		if (extension_manager->has(GlExtensionId::arb_texture_non_power_of_two))
		{
			device_features.npot_is_available_ = true;
		}
	}

	if (!device_features.npot_is_available_)
	{
		extension_manager->probe(GlExtensionId::oes_texture_npot);

		if (extension_manager->has(GlExtensionId::oes_texture_npot))
		{
			device_features.npot_is_available_ = true;
		}
	}
#endif //!BSTONE_RENDERER_3D_TEST_POT_ONLY
}

void GlRenderer3dUtils::mipmap_probe(
	GlExtensionManagerPtr extension_manager,
	Renderer3dDeviceFeatures& device_features,
	GlDeviceFeatures& gl_device_features)
{
	device_features.mipmap_is_available_ = false;
	gl_device_features.mipmap_is_ext_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_SW_MIPMAP
	if (gl_device_features.context_kind_ == GlContextKind::es)
	{
		device_features.mipmap_is_available_ = true;
	}

	if (!device_features.mipmap_is_available_)
	{
		extension_manager->probe(GlExtensionId::arb_framebuffer_object);

		if (extension_manager->has(GlExtensionId::arb_framebuffer_object))
		{
			device_features.mipmap_is_available_ = true;
		}
	}

	if (!device_features.mipmap_is_available_)
	{
		extension_manager->probe(GlExtensionId::ext_framebuffer_object);

		if (extension_manager->has(GlExtensionId::ext_framebuffer_object))
		{
			device_features.mipmap_is_available_ = true;
			gl_device_features.mipmap_is_ext_ = true;
		}
	}
#endif // !BSTONE_RENDERER_3D_TEST_SW_MIPMAP
}

void GlRenderer3dUtils::mipmap_generate(
	const GLenum gl_target,
	const Renderer3dDeviceFeatures& device_features,
	const GlDeviceFeatures& gl_device_features)
{
	if (!device_features.mipmap_is_available_)
	{
		throw Exception{"Mipmap generation not available."};
	}

	switch (gl_target)
	{
		case GL_TEXTURE_2D:
			break;

		default:
			throw Exception{"Unsupported texture target."};
	}

	const auto gl_function = (gl_device_features.mipmap_is_ext_ ? glGenerateMipmapEXT : glGenerateMipmap);

	gl_function(gl_target);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::framebuffer_probe(
	GlExtensionManagerPtr extension_manager,
	GlDeviceFeatures& gl_device_features)
{
	gl_device_features.framebuffer_is_available_ = false;
	gl_device_features.framebuffer_is_ext_ = false;

#ifndef BSTONE_RENDERER_TEST_3D_DEFAULT_FRAMEBUFFER
	if (!gl_device_features.framebuffer_is_available_)
	{
		extension_manager->probe(GlExtensionId::arb_framebuffer_object);

		if (extension_manager->has(GlExtensionId::arb_framebuffer_object))
		{
			gl_device_features.framebuffer_is_available_ = true;
		}
	}

	if (!gl_device_features.framebuffer_is_available_)
	{
		extension_manager->probe(GlExtensionId::ext_framebuffer_blit);
		extension_manager->probe(GlExtensionId::ext_framebuffer_multisample);
		extension_manager->probe(GlExtensionId::ext_framebuffer_object);
		extension_manager->probe(GlExtensionId::ext_packed_depth_stencil);

		if (extension_manager->has(GlExtensionId::ext_framebuffer_blit) &&
			extension_manager->has(GlExtensionId::ext_framebuffer_multisample) &&
			extension_manager->has(GlExtensionId::ext_framebuffer_object) &&
			extension_manager->has(GlExtensionId::ext_packed_depth_stencil))
		{
			gl_device_features.framebuffer_is_available_ = true;
			gl_device_features.framebuffer_is_ext_ = true;
		}
	}
#endif // !BSTONE_RENDERER_TEST_3D_DEFAULT_FRAMEBUFFER
}

void GlRenderer3dUtils::sampler_probe(
	GlExtensionManagerPtr extension_manager,
	Renderer3dDeviceFeatures& device_features)
{
	device_features.sampler_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_SW_SAMPLER
	extension_manager->probe(GlExtensionId::arb_sampler_objects);

	if (extension_manager->has(GlExtensionId::arb_sampler_objects))
	{
		device_features.sampler_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_SW_SAMPLER
}

void GlRenderer3dUtils::sampler_set_anisotropy(
	const GLenum gl_sampler,
	const Renderer3dDeviceFeatures& device_features,
	const int anisotropy_value)
{
	if (!device_features.sampler_is_available_ ||
		!device_features.anisotropy_is_available_)
	{
		return;
	}

	const auto clamped_value = anisotropy_clamp_value(anisotropy_value, device_features);

	const auto gl_value = static_cast<GLfloat>(clamped_value);

	glSamplerParameterf(gl_sampler, GL_TEXTURE_MAX_ANISOTROPY, gl_value);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::vertex_input_vao_probe(
	GlExtensionManagerPtr extension_manager,
	GlDeviceFeatures& gl_device_features)
{
	gl_device_features.vao_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_GL_NO_VAO
	extension_manager->probe(GlExtensionId::arb_vertex_array_object);

	if (extension_manager->has(GlExtensionId::arb_vertex_array_object))
	{
		gl_device_features.vao_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_GL_NO_VAO
}

void GlRenderer3dUtils::vertex_input_probe_max_locations(
	Renderer3dDeviceFeatures& device_features)
{
	auto gl_count = GLint{};

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_count);
	GlError::ensure_debug();

	device_features.vertex_input_max_locations_ = 0;

	if (gl_count > 0)
	{
		device_features.vertex_input_max_locations_ = gl_count;
	}
}

void GlRenderer3dUtils::vsync_probe(
	Renderer3dDeviceFeatures& device_features)
{
	const auto sdl_gl_current_context = ::SDL_GL_GetCurrentContext();

	if (!sdl_gl_current_context)
	{
		throw Exception{"No OpenGL context."};
	}

	device_features.vsync_is_available_ = false;
	device_features.vsync_is_requires_restart_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_NO_SWAP_INTERVAL
	const auto off_result = vsync_set(false);
	const auto on_result = vsync_set(true);

	if (off_result && on_result)
	{
		device_features.vsync_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_NO_SWAP_INTERVAL
}

bool GlRenderer3dUtils::vsync_get()
{
	const auto sdl_result = ::SDL_GL_GetSwapInterval();

	switch (sdl_result)
	{
		case 0:
			return false;

		case -1:
		case 1:
			return true;

		default:
			throw Exception{"Unsupported swap interval value."};
	}
}

bool GlRenderer3dUtils::vsync_set(
	const bool is_enabled)
{
	const auto sdl_result = ::SDL_GL_SetSwapInterval(is_enabled);

	return sdl_result == 0;
}

void GlRenderer3dUtils::buffer_storage_probe(
	const GlExtensionManagerPtr extension_manager,
	GlDeviceFeatures& gl_device_features)
{
	gl_device_features.buffer_storage_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_GL_NO_BUFFER_STORAGE
	extension_manager->probe(GlExtensionId::arb_buffer_storage);

	if (extension_manager->has(GlExtensionId::arb_buffer_storage))
	{
		gl_device_features.buffer_storage_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_GL_NO_BUFFER_STORAGE
}

void GlRenderer3dUtils::dsa_probe(
	const GlExtensionManagerPtr extension_manager,
	GlDeviceFeatures& gl_device_features)
{
	gl_device_features.dsa_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_GL_NO_DSA
	extension_manager->probe(GlExtensionId::arb_direct_state_access);

	if (extension_manager->has(GlExtensionId::arb_direct_state_access))
	{
		gl_device_features.dsa_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_GL_NO_DSA
}

void GlRenderer3dUtils::sso_probe(
	const GlExtensionManagerPtr extension_manager,
	GlDeviceFeatures& gl_device_features)
{
	gl_device_features.sso_is_available_ = false;

#ifndef BSTONE_RENDERER_3D_TEST_GL_NO_SSO
	extension_manager->probe(GlExtensionId::arb_separate_shader_objects);

	if (extension_manager->has(GlExtensionId::arb_separate_shader_objects))
	{
		gl_device_features.sso_is_available_ = true;
	}
#endif // !BSTONE_RENDERER_3D_TEST_GL_NO_SSO
}

void GlRenderer3dUtils::swap_window(
	SdlWindowPtr sdl_window)
{
	assert(sdl_window != nullptr);

	::SDL_GL_SwapWindow(sdl_window);
}

void GlRenderer3dUtils::scissor_enable(
	const bool is_enabled)
{
	if (is_enabled)
	{
		glEnable(GL_SCISSOR_TEST);
		GlError::ensure_debug();
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
		GlError::ensure_debug();
	}
}

void GlRenderer3dUtils::scissor_set_box(
	const Renderer3dScissorBox& scissor_box)
{
	if (scissor_box.x_ < 0)
	{
		throw Exception{"Negative offset by X."};
	}

	if (scissor_box.y_ < 0)
	{
		throw Exception{"Negative offset by Y."};
	}

	if (scissor_box.width_ < 0)
	{
		throw Exception{"Negative width."};
	}

	if (scissor_box.height_ < 0)
	{
		throw Exception{"Negative height."};
	}

	glScissor(
		scissor_box.x_,
		scissor_box.y_,
		scissor_box.width_,
		scissor_box.height_
	);

	GlError::ensure_debug();
}

void GlRenderer3dUtils::viewport_set_rect(
	const Renderer3dViewport& viewport)
{
	if (viewport.x_ < 0)
	{
		throw Exception{"Negative offset by X."};
	}

	if (viewport.y_ < 0)
	{
		throw Exception{"Negative offset by Y."};
	}

	if (viewport.width_ < 0)
	{
		throw Exception{"Negative width."};
	}

	if (viewport.height_ < 0)
	{
		throw Exception{"Negative height."};
	}

	glViewport(
		viewport.x_,
		viewport.y_,
		viewport.width_,
		viewport.height_
	);

	GlError::ensure_debug();
}

void GlRenderer3dUtils::viewport_set_depth_range(
	const Renderer3dViewport& viewport,
	const GlDeviceFeatures& gl_device_features)
{
	if (viewport.min_depth_ < 0.0F || viewport.min_depth_ > 1.0F)
	{
		throw Exception{"Minimum depth out of range."};
	}

	if (viewport.max_depth_ < 0.0F || viewport.max_depth_ > 1.0F)
	{
		throw Exception{"Maximum depth out of range."};
	}

	const auto is_es = (gl_device_features.context_kind_ == GlContextKind::es);

	if (is_es)
	{
		glDepthRangef(viewport.min_depth_, viewport.max_depth_);
		GlError::ensure_debug();
	}
	else
	{
		glDepthRange(viewport.min_depth_, viewport.max_depth_);
		GlError::ensure_debug();
	}
}

void GlRenderer3dUtils::culling_enable(
	const bool is_enable)
{
	const auto gl_function = (is_enable ? glEnable : glDisable);

	gl_function(GL_CULL_FACE);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::culling_set_face(
	const Renderer3dCullingFace culling_face)
{
	auto gl_culling_face = GLenum{};

	switch (culling_face)
	{
	case Renderer3dCullingFace::clockwise:
		gl_culling_face = GL_CW;
		break;

	case Renderer3dCullingFace::counter_clockwise:
		gl_culling_face = GL_CCW;
		break;

	default:
		throw Exception{"Unsupported front face."};
	}

	glFrontFace(gl_culling_face);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::culling_set_mode(
	const Renderer3dCullingMode culling_mode)
{
	auto gl_culling_mode = GLenum{};

	switch (culling_mode)
	{
	case Renderer3dCullingMode::back:
		gl_culling_mode = GL_BACK;
		break;

	case Renderer3dCullingMode::front:
		gl_culling_mode = GL_FRONT;
		break;

	case Renderer3dCullingMode::both:
		gl_culling_mode = GL_FRONT_AND_BACK;
		break;

	default:
		throw Exception{"Unsupported culling mode."};
	}

	glCullFace(gl_culling_mode);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::depth_test_enable(
	const bool is_enable)
{
	const auto gl_function = (is_enable ? glEnable : glDisable);

	gl_function(GL_DEPTH_TEST);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::depth_write_enable(
	const bool is_enable)
{
	glDepthMask(is_enable);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::blending_enable(
	const bool is_enable)
{
	const auto gl_function = (is_enable ? glEnable : glDisable);

	gl_function(GL_BLEND);
	GlError::ensure_debug();
}

void GlRenderer3dUtils::blending_set_func(
	const Renderer3dBlendingFunc& blending_func)
{
	auto gl_src_factor = blending_get_factor(blending_func.src_factor_);
	auto gl_dst_factor = blending_get_factor(blending_func.dst_factor_);

	glBlendFunc(gl_src_factor, gl_dst_factor);
	GlError::ensure_debug();
}

GLenum GlRenderer3dUtils::index_buffer_get_element_type_by_byte_depth(
	const int byte_depth)
{
	switch (byte_depth)
	{
	case 1:
		return GL_UNSIGNED_BYTE;

	case 2:
		return GL_UNSIGNED_SHORT;

	case 4:
		return GL_UNSIGNED_INT;

	default:
		assert(!"Invalid byte depth.");

		return GL_NONE;
	}
}

void GlRenderer3dUtils::renderer_features_set(
	Renderer3dDeviceFeatures& device_features)
{
	// Max texture dimension.
	//
	auto gl_texture_dimension = GLint{};

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_texture_dimension);

	if (gl_texture_dimension == 0)
	{
		throw Exception{"Failed to get maximum texture dimension."};
	}


	// Max viewport dimensions.
	//
	using GlViewportDimensions = std::array<GLint, 2>;
	auto gl_viewport_dimensions = GlViewportDimensions{};

	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, gl_viewport_dimensions.data());

	if (gl_viewport_dimensions[0] == 0 || gl_viewport_dimensions[1] == 0)
	{
		throw Exception{"Failed to get viewport dimensions."};
	}


	// Set the values.
	//
	device_features.texture_max_dimension_ = gl_texture_dimension;

	device_features.viewport_max_width_ = gl_viewport_dimensions[0];
	device_features.viewport_max_height_ = gl_viewport_dimensions[1];
}

Renderer3dDeviceInfo GlRenderer3dUtils::device_info_get()
{
	auto result = Renderer3dDeviceInfo{};


	// Name.
	//
	const auto gl_name = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

	GlError::ensure_debug();

	if (gl_name != nullptr)
	{
		result.name_ = gl_name;
	}


	// Vendor.
	//
	const auto gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

	GlError::ensure_debug();

	if (gl_vendor != nullptr)
	{
		result.vendor_ = gl_vendor;
	}


	// Version.
	//
	const auto gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	GlError::ensure_debug();

	if (gl_version != nullptr)
	{
		result.version_ = gl_version;
	}


	// Result.
	//
	return result;
}

std::string GlRenderer3dUtils::get_log(
	const bool is_shader,
	const GLuint gl_name)
{
	const auto gl_info_function = (is_shader ? glGetShaderInfoLog : glGetProgramInfoLog);
	const auto gl_size_function = (is_shader ? glGetShaderiv : glGetProgramiv);

	auto size = GLint{};

	gl_size_function(gl_name, GL_INFO_LOG_LENGTH, &size);
	GlError::ensure_debug();

	auto result = std::string{};

	if (size > 0)
	{
		result.resize(size);

		auto info_size = GLsizei{};

		gl_info_function(gl_name, size, &info_size, &result[0]);
		GlError::ensure_debug();

		if (info_size <= 0)
		{
			result.clear();
		}
	}

	return result;
}

GLenum GlRenderer3dUtils::filter_get_mag(
	const Renderer3dFilterKind mag_filter)
{
	switch (mag_filter)
	{
	case Renderer3dFilterKind::nearest:
		return GL_NEAREST;

	case Renderer3dFilterKind::linear:
		return GL_LINEAR;

	default:
		throw Exception{"Unsupported magnification filter."};
	}
}

GLenum GlRenderer3dUtils::filter_get_min(
	const Renderer3dFilterKind min_filter,
	const Renderer3dMipmapMode mipmap_mode)
{
	switch (mipmap_mode)
	{
	case Renderer3dMipmapMode::none:
		switch (min_filter)
		{
		case Renderer3dFilterKind::nearest:
			return GL_NEAREST;

		case Renderer3dFilterKind::linear:
			return GL_LINEAR;

		default:
			throw Exception{"Unsupported minification filter."};
		}

		break;

	case Renderer3dMipmapMode::nearest:
		switch (min_filter)
		{
		case Renderer3dFilterKind::nearest:
			return GL_NEAREST_MIPMAP_NEAREST;

		case Renderer3dFilterKind::linear:
			return GL_LINEAR_MIPMAP_NEAREST;

		default:
			throw Exception{"Unsupported minification mipmap filter."};
		}

		break;

	case Renderer3dMipmapMode::linear:
		switch (min_filter)
		{
		case Renderer3dFilterKind::nearest:
			return GL_NEAREST_MIPMAP_LINEAR;

		case Renderer3dFilterKind::linear:
			return GL_LINEAR_MIPMAP_LINEAR;

		default:
			throw Exception{"Unsupported minification mipmap filter."};
		}

		break;

	default:
		throw Exception{"Unsupported mipmap mode."};
	}
}

GLenum GlRenderer3dUtils::address_mode_get(
	const Renderer3dAddressMode address_mode)
{
	switch (address_mode)
	{
	case Renderer3dAddressMode::clamp:
		return GL_CLAMP_TO_EDGE;

	case Renderer3dAddressMode::repeat:
		return GL_REPEAT;

	default:
		throw Exception{"Unsupported address mode."};
	}
}

GLenum GlRenderer3dUtils::texture_wrap_get_axis(
	const Renderer3dTextureAxis texture_axis)
{
	switch (texture_axis)
	{
		case Renderer3dTextureAxis::u:
			return GL_TEXTURE_WRAP_S;

		case Renderer3dTextureAxis::v:
			return GL_TEXTURE_WRAP_T;

		default:
			throw Exception{"Unsupported texture axis."};
	}
}

GLenum GlRenderer3dUtils::blending_get_factor(
	const Renderer3dBlendingFactor factor)
{
	switch (factor)
	{
		case Renderer3dBlendingFactor::zero:
			return GL_ZERO;

		case Renderer3dBlendingFactor::one:
			return GL_ONE;

		case Renderer3dBlendingFactor::src_color:
			return GL_SRC_COLOR;

		case Renderer3dBlendingFactor::src_alpha:
			return GL_SRC_ALPHA;

		case Renderer3dBlendingFactor::one_minus_src_alpha:
			return GL_ONE_MINUS_SRC_ALPHA;

		default:
			return GL_NONE;
	}
}

//
// GlRenderer3dUtils
// ==========================================================================


} // detail
} // bstone
