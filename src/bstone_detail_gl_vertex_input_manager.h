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
// OpenGL vertex input manager (implementation interface).
//


#ifndef BSTONE_DETAIL_GL_VERTEX_INPUT_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_VERTEX_INPUT_MANAGER_INCLUDED


#include <memory>

#include "bstone_renderer_3d_vertex_input.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// ==========================================================================
// GlVertexInputManager
//

class GlVertexInputManager
{
protected:
	GlVertexInputManager() = default;


public:
	virtual ~GlVertexInputManager() = default;


	virtual GlContextPtr get_gl_context() const noexcept = 0;


	virtual Renderer3dVertexInputUPtr create(
		const Renderer3dVertexInputCreateParam& param) = 0;

	virtual void notify_destroy(
		const Renderer3dVertexInputPtr vertex_input) noexcept = 0;

	virtual void set(
		const Renderer3dVertexInputPtr vertex_input) = 0;

	virtual Renderer3dBufferPtr get_current_index_buffer() const noexcept = 0;


	virtual void enable_location(
		const int location,
		const bool is_enabled) = 0;

	virtual void location_assign_begin() = 0;

	virtual void location_assign_end() = 0;
}; // GlVertexInputManager

using GlVertexInputManagerPtr = GlVertexInputManager*;
using GlVertexInputManagerUPtr = std::unique_ptr<GlVertexInputManager>;

//
// GlVertexInputManager
// ==========================================================================


// ==========================================================================
// GlVertexInputManagerFactory
//

struct GlVertexInputManagerFactory
{
	static GlVertexInputManagerUPtr create(
		const GlContextPtr gl_context);
}; // GlVertexInputManagerFactory

//
// GlVertexInputManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_VERTEX_INPUT_MANAGER_INCLUDED
