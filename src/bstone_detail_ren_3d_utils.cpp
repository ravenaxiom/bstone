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
// 3D renderer utils.
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_ren_3d_utils.h"

#include <algorithm>
#include <unordered_set>

#include "bstone_exception.h"
#include "bstone_ren_3d_limits.h"
#include "bstone_sprite.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dUtils
//

float Ren3dUtils::deg_to_rad(
	const float angle_deg)
{
	return angle_deg * pi_over_180;
}

bool Ren3dUtils::is_pot_value(
		const int value)
{
	const auto nearest_value = find_nearest_pot_value(value);

	return nearest_value == value;
}

int Ren3dUtils::find_nearest_pot_value(
	const int value)
{
	for (int i = 0; i < 32; ++i)
	{
		const auto new_value = 1 << i;

		if (new_value >= value)
		{
			return new_value;
		}
	}

	return 0;
}

int Ren3dUtils::calculate_mipmap_count(
	const int width,
	const int height)
{
	//
	// mipmap_count = [log2(max(width, height))] + 1
	//

	const auto max_size = std::max(width, height);

	int log_2;

	for (log_2 = 0; log_2 < 32; ++log_2)
	{
		if ((1 << log_2) >= max_size)
		{
			break;
		}
	}

	const auto result = log_2 + 1;

	return result;
}

SdlWindowUPtr Ren3dUtils::window_create(
	const Ren3dUtilsCreateWindowParam& param)
{
	const auto error_message_prefix = "Failed to create a window. ";

	create_window_validate_param(param);
	create_window_set_gl_attributes(param);

	const auto sdl_flags = create_window_sdl_flags(param);

	const auto x = (
		param.window_.is_positioned_ ?
			param.window_.rect_2d_.offset_.x_
			:
			SDL_WINDOWPOS_CENTERED
	);

	const auto y = (
		param.window_.is_positioned_ ?
			param.window_.rect_2d_.offset_.y_
			:
			SDL_WINDOWPOS_CENTERED
	);

	auto sdl_window = SdlWindowUPtr{::SDL_CreateWindow(
		param.window_.title_.c_str(),
		x,
		y,
		param.window_.rect_2d_.extent_.width_,
		param.window_.rect_2d_.extent_.height_,
		sdl_flags
	)};

	if (!sdl_window)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += ::SDL_GetError();

		throw Exception{error_message};
	}

	return sdl_window;
}

void Ren3dUtils::set_window_mode(
	SdlWindowPtr sdl_window,
	const Ren3dSetWindowModeParam& param)
{
	if (!sdl_window)
	{
		throw Exception{"Null window."};
	}

	if (param.rect_2d_.extent_.height_ <= 0)
	{
		throw Exception{"Height out of range."};
	}

	if (param.rect_2d_.extent_.width_ <= 0)
	{
		throw Exception{"Width out of range."};
	}


	//
	const auto sdl_window_flags = ::SDL_GetWindowFlags(sdl_window);

	const auto is_current_windowed =
		((sdl_window_flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) == 0);

	const auto is_windowed_changed = (is_current_windowed != param.is_windowed_);


	//
	int current_width = 0;
	int current_height = 0;

	::SDL_GetWindowSize(sdl_window, &current_width, &current_height);

	if (current_width <= 0 || current_height <= 0)
	{
		throw Exception{"Failed to get current window size."};
	}

	const auto is_size_changed = (
		current_width != param.rect_2d_.extent_.width_ ||
		current_height != param.rect_2d_.extent_.height_);


	//
	if (!is_windowed_changed && !is_size_changed)
	{
		return;
	}

	if (is_windowed_changed)
	{
		const Uint32 sdl_window_flags = (param.is_windowed_ ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);

		const auto sdl_result = ::SDL_SetWindowFullscreen(sdl_window, sdl_window_flags);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to change fullscreen mode."};
		}
	}

	if (is_size_changed && param.is_windowed_)
	{
		::SDL_SetWindowSize(sdl_window, param.rect_2d_.extent_.width_, param.rect_2d_.extent_.height_);

		if (param.is_positioned_)
		{
			const auto x = std::max(param.rect_2d_.offset_.x_, 0);
			const auto y = std::max(param.rect_2d_.offset_.y_, 0);

			::SDL_SetWindowPosition(sdl_window, x, y);
		}
		else
		{
			::SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
}

void Ren3dUtils::show_window(
	SdlWindowPtr sdl_window,
	const bool is_visible)
{
	const auto error_message_prefix = "Failed to show a window. ";

	if (!sdl_window)
	{
		auto error_message = std::string{error_message_prefix};
		error_message += "Null SDL window.";

		throw Exception{error_message};
	}

	const auto sdl_function = (is_visible ? ::SDL_ShowWindow : ::SDL_HideWindow);

	sdl_function(sdl_window);
}

void Ren3dUtils::set_window_title(
	const SdlWindowPtr sdl_window,
	const std::string& title_utf8)
{
	if (!sdl_window)
	{
		throw Exception{"No window."};
	}

	::SDL_SetWindowTitle(sdl_window, title_utf8.c_str());
}

void Ren3dUtils::validate_initialize_param(
	const Ren3dCreateParam& param)
{
	switch (param.renderer_kind_)
	{
	case Ren3dKind::gl_2_0:
	case Ren3dKind::gl_3_2_core:
	case Ren3dKind::gles_2_0:
		break;

	default:
		throw Exception{"Unsupported renderer kind."};
	}

	if (param.window_.rect_2d_.extent_.width_ <= 0)
	{
		throw Exception{"Invalid window width."};
	}

	if (param.window_.rect_2d_.extent_.height_ <= 0)
	{
		throw Exception{"Invalid window height."};
	}

	switch (param.aa_kind_)
	{
		case Ren3dAaKind::none:
		case Ren3dAaKind::ms:
			break;

		default:
			throw Exception{"Invalid antialiasing kind."};
	}
}

void Ren3dUtils::validate_buffer_create_param(
	const Ren3dCreateBufferParam& param)
{
	switch (param.usage_kind_)
	{
		case Ren3dBufferUsageKind::draw_streaming:
		case Ren3dBufferUsageKind::draw_static:
		case Ren3dBufferUsageKind::draw_dynamic:
			break;

		default:
			throw Exception{"Invalid usage kind."};
	}

	if (param.size_ <= 0)
	{
		throw Exception{"Invalid size."};
	}
}

void Ren3dUtils::validate_buffer_update_param(
	const Ren3dUpdateBufferParam& param)
{
	if (param.offset_ < 0)
	{
		throw Exception{"Invalid offset."};
	}

	if (param.size_ <= 0)
	{
		throw Exception{"Invalid count."};
	}

	if (!param.data_)
	{
		throw Exception{"Null indices."};
	}
}

void Ren3dUtils::vertex_input_validate_format(
	const Ren3dVertexAttribFormat attribute_format)
{
	switch (attribute_format)
	{
		case Ren3dVertexAttribFormat::rgba_8_unorm:
		case Ren3dVertexAttribFormat::rg_32_sfloat:
		case Ren3dVertexAttribFormat::rgb_32_sfloat:
			return;

		default:
			throw Exception{"Unsupported vertex input attribute format."};
	}
}

void Ren3dUtils::vertex_input_validate_param(
	const int max_locations,
	const Ren3dCreateVertexInputParam& param)
{
	if (max_locations < 0)
	{
		throw Exception{"Maximum location count out of range."};
	}

	if (!param.index_buffer_)
	{
		throw Exception{"Null index buffer."};
	}

	if (param.index_buffer_->get_kind() != Ren3dBufferKind::index)
	{
		throw Exception{"Non-index buffer."};
	}

	const auto& attribute_descriptions = param.attribute_descriptions_;

	if (attribute_descriptions.empty())
	{
		throw Exception{"No descriptions."};
	}

	using UsedLocations = std::unordered_set<int>;
	auto used_locations = UsedLocations{};
	used_locations.reserve(max_locations);

	for (const auto& attribute_description : attribute_descriptions)
	{
		if (attribute_description.is_default_)
		{
			continue;
		}

		// Location.
		//
		const auto location = attribute_description.location_;

		if (location < 0 || location >= max_locations)
		{
			throw Exception{"Location out of range."};
		}

		const auto has_location = (used_locations.find(location) != used_locations.cend());

		if (has_location)
		{
			throw Exception{"Duplicate location."};
		}

		used_locations.insert(location);


		// Format.
		//
		vertex_input_validate_format(attribute_description.format_);


		// Vertex buffer.
		//
		if (!attribute_description.vertex_buffer_)
		{
			throw Exception{"Null vertex buffer."};
		}

		if (attribute_description.vertex_buffer_->get_kind() != Ren3dBufferKind::vertex)
		{
			throw Exception{"Non-vertex buffer."};
		}


		// Offset.
		//
		if (attribute_description.offset_ < 0)
		{
			throw Exception{"Invalid offset."};
		}


		// Stride.
		//
		if (attribute_description.stride_ < 0)
		{
			throw Exception{"Invalid stride."};
		}
	}
}

void Ren3dUtils::indexed_to_rgba_8(
	const IndexedToRgba8Param& param)
{
	if (param.width_ <= 0)
	{
		throw Exception{"Width out of range."};
	}

	if (param.height_ <= 0)
	{
		throw Exception{"Height out of range."};
	}

	if (!param.indexed_pixels_)
	{
		throw Exception{"Null indexed pixels."};
	}

	if (param.rgba_8_buffer_->size() < (param.width_ * param.height_))
	{
		throw Exception{"Bitmap buffer too small."};
	}

	const auto is_column_major = param.indexed_is_column_major_;
	const auto has_alphas = (param.indexed_alphas_ != nullptr);

	if (!is_column_major && !has_alphas)
	{
		indexed_to_rgba_8_rm_na(param);
	}
	else if (!is_column_major && has_alphas)
	{
		indexed_to_rgba_8_rm_ha(param);
	}
	else if (is_column_major && !has_alphas)
	{
		indexed_to_rgba_8_cm_na(param);
	}
	else if (is_column_major && has_alphas)
	{
		indexed_to_rgba_8_cm_ha(param);
	}
	else
	{
		throw Exception{"Unsupported values combination."};
	}
}

void Ren3dUtils::indexed_npot_to_rgba_8_pot(
	const IndexedToRgba8Param& param)
{
	if (param.width_ <= 0)
	{
		throw Exception{"Width out of range."};
	}

	if (param.height_ <= 0)
	{
		throw Exception{"Height out of range."};
	}

	if (param.actual_width_ <= 0)
	{
		throw Exception{"Actual width out of range."};
	}

	if (param.actual_height_ <= 0)
	{
		throw Exception{"Actual height out of range."};
	}

	if (!param.indexed_pixels_)
	{
		throw Exception{"Null indexed pixels."};
	}

	if (param.rgba_8_buffer_->size() < (param.actual_width_ * param.actual_height_))
	{
		throw Exception{"Indexed bitmap buffer too small."};
	}

	const auto is_column_major = param.indexed_is_column_major_;
	const auto has_alphas = (param.indexed_alphas_ != nullptr);

	if (!is_column_major && !has_alphas)
	{
		indexed_npot_to_rgba_8_rm_na(param);
	}
	else if (!is_column_major && has_alphas)
	{
		indexed_npot_to_rgba_8_rm_ha(param);
	}
	else if (is_column_major && !has_alphas)
	{
		indexed_npot_to_rgba_8_cm_na(param);
	}
	else if (is_column_major && has_alphas)
	{
		indexed_npot_to_rgba_8_cm_ha(param);
	}
	else
	{
		throw Exception{"Unsupported values combination."};
	}
}

void Ren3dUtils::indexed_to_rgba_8_pot(
	const IndexedToRgba8Param& param)
{
	const auto is_npot = (
		param.width_ != param.actual_width_ ||
		param.height_ != param.actual_height_
	);

	if (!is_npot)
	{
		indexed_to_rgba_8(param);
	}
	else if (is_npot)
	{
		indexed_npot_to_rgba_8_pot(param);
	}
}

void Ren3dUtils::indexed_sprite_to_rgba_8_pot(
	const Sprite& indexed_sprite,
	const Rgba8Palette& indexed_palette,
	Rgba8Buffer& texture_buffer)
{
	if (!indexed_sprite.is_initialized())
	{
		throw Exception{"Indexed sprite not initialized."};
	}

	const auto left = indexed_sprite.get_left();
	const auto right = indexed_sprite.get_right();
	const auto top = indexed_sprite.get_top();
	const auto bottom = indexed_sprite.get_bottom();

	for (int w = 0; w < Sprite::dimension; ++w)
	{
		const std::int16_t* column = nullptr;

		if (w >= left && w <= right)
		{
			column = indexed_sprite.get_column(w - left);
		}

		auto dst_pixels = &texture_buffer[w];

		for (int h = 0; h < Sprite::dimension; ++h)
		{
			auto& dst_pixel = *dst_pixels;

			if (column && h >= top && h <= bottom)
			{
				const auto src_index = h - top;

				const auto src_pixel = column[src_index];

				if (src_pixel < 0)
				{
					dst_pixel.reset();
				}
				else
				{
					dst_pixel = indexed_palette[src_pixel];
				}
			}
			else
			{
				dst_pixel.reset();
			}

			dst_pixels += Sprite::dimension;
		}
	}
}

void Ren3dUtils::rgba_8_npot_to_rgba_8_pot(
	const int width,
	const int height,
	const int actual_width,
	const int actual_height,
	const Rgba8* const rgba_8_pixels,
	Rgba8Buffer& texture_buffer)
{
	if (width <= 0)
	{
		throw Exception{"Width out of range."};
	}

	if (height <= 0)
	{
		throw Exception{"Height out of range."};
	}

	if (actual_width <= 0)
	{
		throw Exception{"Actual width out of range."};
	}

	if (actual_height <= 0)
	{
		throw Exception{"Actual height out of range."};
	}

	if (!rgba_8_pixels)
	{
		throw Exception{"Null RGBA pixels."};
	}

	const auto src_du_f = static_cast<double>(width) / static_cast<double>(actual_width);
	const auto src_dv_f = static_cast<double>(height) / static_cast<double>(actual_height);

	auto src_v_d = 0.0;

	auto dst_index = 0;

	for (int h = 0; h < actual_height; ++h)
	{
		const auto src_v = static_cast<int>(src_v_d);

		auto src_u_d = 0.0;

		for (int w = 0; w < actual_width; ++w)
		{
			const auto src_u = static_cast<int>(src_u_d);

			const auto src_index = (src_v * width) + src_u;

			texture_buffer[dst_index] = rgba_8_pixels[src_index];

			++dst_index;

			src_u_d += src_du_f;
		}

		src_v_d += src_dv_f;
	}
}

void Ren3dUtils::build_mipmap(
	const int previous_width,
	const int previous_height,
	const Rgba8CPtr src_colors,
	const Rgba8Ptr dst_colors)
{
	if (previous_width <= 0)
	{
		throw Exception{"Previous width out of range."};
	}

	if (previous_height <= 0)
	{
		throw Exception{"Previous height out of range."};
	}

	if (previous_width == 1 && previous_height == 1)
	{
		throw Exception{"No more mipmap to build."};
	}

	if (!src_colors)
	{
		throw Exception{"Null source colors."};
	}

	if (!dst_colors)
	{
		throw Exception{"Null target colors."};
	}

	if (previous_width == 1)
	{
		build_mipmap_1(previous_height, src_colors, dst_colors);

		return;
	}

	if (previous_height == 1)
	{
		build_mipmap_1(previous_width, src_colors, dst_colors);

		return;
	}

	const auto width = previous_width / 2;
	const auto height = previous_height / 2;

	auto dst_colors_0 = dst_colors;

	auto src_colors_0 = src_colors;
	auto src_colors_1 = src_colors_0 + previous_width;

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			*dst_colors_0++ = Rgba8::average_pa(
				src_colors_0[0],
				src_colors_0[1],
				src_colors_1[0],
				src_colors_1[1]
			);

			src_colors_0 += 2;
			src_colors_1 += 2;
		}

		src_colors_0 += previous_width;
		src_colors_1 += previous_width;
	}
}

void Ren3dUtils::create_window_validate_param(
	const Ren3dUtilsCreateWindowParam& param)
{
	if (param.window_.is_positioned_ && (
		param.window_.rect_2d_.offset_.x_ < 0 ||
			param.window_.rect_2d_.offset_.y_ < 0))
	{
		throw Exception{"Invalid position."};
	}

	if (param.window_.rect_2d_.extent_.width_ <= 0 ||
		param.window_.rect_2d_.extent_.height_ <= 0)
	{
		throw Exception{"Invalid dimensions."};
	}
}

void Ren3dUtils::create_window_set_gl_profile_and_version(
	const Ren3dKind renderer_kind)
{
	auto sdl_profile_mask = 0;
	auto sdl_version_major = 0;
	auto sdl_version_minor = 0;

	switch (renderer_kind)
	{
		case Ren3dKind::gl_2_0:
			sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
			sdl_version_major = 2;
			sdl_version_minor = 0;
			break;

		case Ren3dKind::gl_3_2_core:
			sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
			sdl_version_major = 3;
			sdl_version_minor = 2;
			break;

		case Ren3dKind::gles_2_0:
			sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
			sdl_version_major = 2;
			sdl_version_minor = 0;
			break;

		default:
			throw Exception{"Unsupported 3D-renderer kind."};
	}

	{
		const auto sdl_result = ::SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			sdl_profile_mask
		);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set OpenGL context core profile attribute."};
		}
	}

	{
		const auto sdl_result = ::SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_MAJOR_VERSION,
			sdl_version_major
		);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set OpenGL context major version attribute."};
		}
	}

	{
		const auto sdl_result = ::SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_MINOR_VERSION,
			sdl_version_minor
		);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set OpenGL context minor version attribute."};
		}
	}
}

void Ren3dUtils::create_window_set_gl_attributes(
	const Ren3dUtilsCreateWindowParam& param)
{
	::SDL_GL_ResetAttributes();

	auto sdl_result = 0;

	switch (param.aa_kind_)
	{
		case Ren3dAaKind::ms:
			if (param.aa_value_ >= Ren3dLimits::aa_min_on)
			{
				sdl_result = ::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);

				if (sdl_result != 0)
				{
					throw Exception{"Failed to set multisample buffer count."};
				}

				sdl_result = ::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, param.aa_value_);

				if (sdl_result != 0)
				{
					throw Exception{"Failed to set multisample sample count."};
				}
			}

			break;


		case Ren3dAaKind::none:
		default:
			sdl_result = ::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);

			if (sdl_result != 0)
			{
				throw Exception{"Failed to set multisample buffer count."};
			}

			sdl_result = ::SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

			if (sdl_result != 0)
			{
				throw Exception{"Failed to set multisample sample count."};
			}

			break;
	}

	{
		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set a red bit depth."};
		}
	}

	{
		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set a green bit depth."};
		}
	}

	{
		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set a blue bit depth."};
		}
	}

	{
		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set an alpha bit depth."};
		}
	}

	{
		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set an accelerated visual."};
		}
	}

	{
		const auto gl_depth_size = (
			param.is_default_depth_buffer_disabled_ ?
			0 :
			16
		);

		sdl_result = ::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, gl_depth_size);

		if (sdl_result != 0)
		{
			throw Exception{"Failed to set depth buffer bit depth."};
		}
	}

	create_window_set_gl_profile_and_version(param.renderer_kind_);
}

std::uint32_t Ren3dUtils::create_window_sdl_flags(
	const Ren3dUtilsCreateWindowParam& param)
{
	auto flags = Uint32{};

	if (param.window_.is_visible_)
	{
		flags |= SDL_WINDOW_SHOWN;
	}
	else
	{
		flags |= SDL_WINDOW_HIDDEN;
	}

	if (param.window_.is_fake_fullscreen_)
	{
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	if (param.window_.is_borderless_)
	{
		flags |= SDL_WINDOW_BORDERLESS;
	}

	switch (param.renderer_kind_)
	{
		case Ren3dKind::gl_2_0:
		case Ren3dKind::gl_3_2_core:
		case Ren3dKind::gles_2_0:
			flags |= SDL_WINDOW_OPENGL;
			break;

		default:
			break;
	}

	return flags;
}

// Indexed (row major, has no alpha) -> RGBA
void Ren3dUtils::indexed_to_rgba_8_rm_na(
	const IndexedToRgba8Param& param)
{
	auto src_pixels = param.indexed_pixels_;
	const auto& src_palette = *param.indexed_palette_;

	auto dst_pixels = param.rgba_8_buffer_->data();

	const auto area = param.width_ * param.height_;

	for (int i = 0; i < area; ++i)
	{
		*dst_pixels++ = src_palette[*src_pixels++];
	}
}

// Indexed (row major, has alpha) -> RGBA
void Ren3dUtils::indexed_to_rgba_8_rm_ha(
	const IndexedToRgba8Param& param)
{
	auto src_pixels = param.indexed_pixels_;
	const auto& src_palette = *param.indexed_palette_;

	auto dst_pixels = param.rgba_8_buffer_->data();

	const auto area = param.width_ * param.height_;

	for (int i = 0; i < area; ++i)
	{
		const auto src_pixel = *src_pixels++;
		auto& dst_pixel = *dst_pixels++;

		if (param.indexed_alphas_[i])
		{
			dst_pixel = src_palette[src_pixel];
		}
		else
		{
			dst_pixel.reset();
		}
	}
}

// Indexed (column major, has no alpha) -> RGBA
void Ren3dUtils::indexed_to_rgba_8_cm_na(
	const IndexedToRgba8Param& param)
{
	const auto& src_palette = *param.indexed_palette_;

	auto dst_pixels = param.rgba_8_buffer_->data();

	for (int src_y = 0; src_y < param.height_; ++src_y)
	{
		auto src_index = src_y;

		for (int src_x = 0; src_x < param.width_; ++src_x)
		{
			*dst_pixels++ = src_palette[param.indexed_pixels_[src_index]];

			src_index += param.height_;
		}
	}
}

// Indexed (column major, has alpha) -> RGBA
void Ren3dUtils::indexed_to_rgba_8_cm_ha(
	const IndexedToRgba8Param& param)
{
	const auto& src_palette = *param.indexed_palette_;

	auto dst_pixels = param.rgba_8_buffer_->data();

	for (int src_y = 0; src_y < param.height_; ++src_y)
	{
		auto src_index = src_y;

		for (int src_x = 0; src_x < param.width_; ++src_x)
		{
			auto& dst_pixel = *dst_pixels++;

			if (param.indexed_alphas_[src_index])
			{
				dst_pixel = src_palette[param.indexed_pixels_[src_index]];
			}
			else
			{
				dst_pixel.reset();
			}

			src_index += param.height_;
		}
	}
}

// Indexed (row major, has no alpha) -> RGBA POT
void Ren3dUtils::indexed_npot_to_rgba_8_rm_na(
	const IndexedToRgba8Param& param)
{
	const auto src_du_f = static_cast<double>(param.width_) / static_cast<double>(param.actual_width_);
	const auto src_dv_f = static_cast<double>(param.height_) / static_cast<double>(param.actual_height_);

	const auto& src_palette = *param.indexed_palette_;
	auto dst_colors = param.rgba_8_buffer_->data();

	auto src_v_f = 0.0;
	auto cached_src_index = -1;
	auto cached_color = Rgba8{};

	for (int h = 0; h < param.actual_height_; ++h)
	{
		const auto src_v = static_cast<int>(src_v_f);
		const auto src_base_index = src_v * param.width_;

		auto src_u_f = 0.0;

		for (int w = 0; w < param.actual_width_; ++w)
		{
			const auto src_u = static_cast<int>(src_u_f);
			const auto src_index = src_base_index + src_u;

			if (src_index != cached_src_index)
			{
				cached_src_index = src_index;
				cached_color = src_palette[param.indexed_pixels_[src_index]];
			}

			*dst_colors++ = cached_color;

			src_u_f += src_du_f;
		}

		src_v_f += src_dv_f;
	}
}

// Indexed (row major, has alpha) -> RGBA POT
void Ren3dUtils::indexed_npot_to_rgba_8_rm_ha(
	const IndexedToRgba8Param& param)
{
	const auto src_du_f = static_cast<double>(param.width_) / static_cast<double>(param.actual_width_);
	const auto src_dv_f = static_cast<double>(param.height_) / static_cast<double>(param.actual_height_);

	const auto& src_palette = *param.indexed_palette_;
	auto dst_colors = param.rgba_8_buffer_->data();

	auto src_v_f = 0.0;
	auto cached_src_index = -1;
	auto cached_color = Rgba8{};

	for (int h = 0; h < param.actual_height_; ++h)
	{
		const auto src_v = static_cast<int>(src_v_f);
		const auto src_base_index = src_v * param.width_;

		auto src_u_f = 0.0;

		for (int w = 0; w < param.actual_width_; ++w)
		{
			const auto src_u = static_cast<int>(src_u_f);
			const auto src_index = src_base_index + src_u;

			auto& dst_pixel = *dst_colors++;

			if (param.indexed_alphas_[src_index])
			{
				if (src_index != cached_src_index)
				{
					cached_src_index = src_index;
					cached_color = src_palette[param.indexed_pixels_[src_index]];
				}

				dst_pixel = cached_color;
			}
			else
			{
				dst_pixel.reset();
			}

			src_u_f += src_du_f;
		}

		src_v_f += src_dv_f;
	}
}

// Indexed (column major, has no alpha) -> RGBA POT
void Ren3dUtils::indexed_npot_to_rgba_8_cm_na(
	const IndexedToRgba8Param& param)
{
	const auto src_du_f = static_cast<double>(param.width_) / static_cast<double>(param.actual_width_);
	const auto src_dv_f = static_cast<double>(param.height_) / static_cast<double>(param.actual_height_);

	const auto& src_palette = *param.indexed_palette_;
	auto dst_colors = param.rgba_8_buffer_->data();

	auto src_v_f = 0.0;
	auto cached_src_index = -1;
	auto cached_color = Rgba8{};

	for (int h = 0; h < param.actual_height_; ++h)
	{
		const auto src_v = static_cast<int>(src_v_f);

		auto src_u_f = 0.0;

		for (int w = 0; w < param.actual_width_; ++w)
		{
			const auto src_u = static_cast<int>(src_u_f);

			const auto src_index = (src_u * param.height_) + src_v;

			if (src_index != cached_src_index)
			{
				cached_src_index = src_index;
				cached_color = src_palette[param.indexed_pixels_[src_index]];
			}

			*dst_colors++ = cached_color;

			src_u_f += src_du_f;
		}

		src_v_f += src_dv_f;
	}
}

// Indexed (column major, has alpha) -> RGBA POT
void Ren3dUtils::indexed_npot_to_rgba_8_cm_ha(
	const IndexedToRgba8Param& param)
{
	const auto src_du_f = static_cast<double>(param.width_) / static_cast<double>(param.actual_width_);
	const auto src_dv_f = static_cast<double>(param.height_) / static_cast<double>(param.actual_height_);

	const auto& src_palette = *param.indexed_palette_;
	auto dst_colors = param.rgba_8_buffer_->data();

	auto src_v_f = 0.0;
	auto cached_src_index = -1;
	auto cached_color = Rgba8{};

	for (int h = 0; h < param.actual_height_; ++h)
	{
		const auto src_v = static_cast<int>(src_v_f);

		auto src_u_f = 0.0;

		for (int w = 0; w < param.actual_width_; ++w)
		{
			const auto src_u = static_cast<int>(src_u_f);

			const auto src_index = (src_u * param.height_) + src_v;

			auto& dst_pixel = *dst_colors++;

			if (param.indexed_alphas_[src_index])
			{
				if (src_index != cached_src_index)
				{
					cached_src_index = src_index;
					cached_color = src_palette[param.indexed_pixels_[src_index]];
				}

				dst_pixel = cached_color;
			}
			else
			{
				dst_pixel.reset();
			}

			src_u_f += src_du_f;
		}

		src_v_f += src_dv_f;
	}
}

void Ren3dUtils::build_mipmap_1(
	const int previous_dimension,
	const Rgba8CPtr src_colors,
	const Rgba8Ptr dst_colors)
{
	const auto dimension = previous_dimension / 2;

	auto src_colors_0 = src_colors;
	auto dst_colors_0 = dst_colors;

	for (int i = 0; i < dimension; ++i)
	{
		*dst_colors_0++ = Rgba8::average_pa(
			src_colors_0[0],
			src_colors_0[1]
		);

		src_colors_0 += 2;
	}
}

//
// Ren3dUtils
// ==========================================================================


} // detail
} // bstone
