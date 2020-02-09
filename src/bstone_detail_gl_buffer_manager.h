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
// OpenGL buffer object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_GL_BUFFER_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_BUFFER_MANAGER_INCLUDED


#include <memory>

#include "bstone_renderer_3d_buffer.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;

class GlVaoManager;
using GlVaoManagerPtr = GlVaoManager*;


// ==========================================================================
// GlBufferManager
//

class GlBufferManager
{
protected:
	GlBufferManager() = default;


public:
	virtual ~GlBufferManager() = default;


	virtual GlContextPtr get_context() const noexcept = 0;


	virtual Renderer3dBufferUPtr buffer_create(
		const Renderer3dBufferCreateParam& param) = 0;

	virtual void buffer_notify_destroy(
		const Renderer3dBufferPtr buffer) noexcept = 0;


	virtual bool buffer_set_current(
		const Renderer3dBufferKind buffer_kind,
		const Renderer3dBufferPtr buffer) = 0;
}; // GlBufferManager

using GlBufferManagerPtr = GlBufferManager*;
using GlBufferManagerUPtr = std::unique_ptr<GlBufferManager>;

//
// GlBufferManager
// ==========================================================================


// ==========================================================================
// GlBufferManagerFactory
//

struct GlBufferManagerFactory
{
	static GlBufferManagerUPtr create(
		const GlContextPtr gl_context,
		const GlVaoManagerPtr gl_vao_manager);
}; // GlBufferManagerFactory

//
// GlBufferManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_BUFFER_MANAGER_INCLUDED
