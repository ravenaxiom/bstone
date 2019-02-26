/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

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
// Renderer utils.
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_RENDERER_UTILS_INCLUDED
#define BSTONE_RENDERER_UTILS_INCLUDED


#include <string>
#include "bstone_renderer.h"


struct SDL_Window;
using SdlWindowPtr = SDL_Window*;


namespace bstone
{


class RendererUtilsCreateWindowParam
{
public:
	bool is_opengl_;

	RendererInitializeWindowParam window_;
}; // RendererUtilsCreateWindowParam


class RendererUtils
{
public:
	const std::string& get_error_message() const;


	static int find_nearest_pot_value(
		const int value);

	bool create_window(
		const RendererUtilsCreateWindowParam& param,
		SdlWindowPtr& sdl_window);

	bool show_window(
		SdlWindowPtr sdl_window,
		const bool is_visible);

	bool validate_initialize_param(
		const RendererInitializeParam& param);


	bool validate_index_buffer_create_param(
		const RendererIndexBufferCreateParam& param);

	bool validate_index_buffer_update_param(
		const RendererIndexBufferUpdateParam& param);


	bool validate_vertex_buffer_create_param(
		const RendererVertexBufferCreateParam& param);

	bool validate_vertex_buffer_update_param(
		const RendererVertexBufferUpdateParam& param);


	bool validate_texture_2d_create_param(
		const RendererTexture2dCreateParam& param);

	bool validate_texture_2d_update_param(
		const RendererTexture2dUpdateParam& param);

	static bool is_ogl_renderer_path(
		const RendererPath renderer_path);


private:
	std::string error_message_;


	bool create_window_validate_param(
		const RendererUtilsCreateWindowParam& param);

	bool create_window_set_ogl_attributes();

	std::uint32_t create_window_sdl_flags(
		const RendererUtilsCreateWindowParam& param);
}; // RendererUtils


} // bstone


#endif // !BSTONE_RENDERER_UTILS_INCLUDED
