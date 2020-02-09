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
// OpenGL buffer object manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_buffer_mgr.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_buffer.h"
#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_vao_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlBufferManagerImplException
//

struct GlBufferManagerImplException :
	public Exception
{
	explicit GlBufferManagerImplException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR] "} + message}
	{
	}
}; // GlBufferManagerImplException

//
// GlBufferManagerImplException
// ==========================================================================


// ==========================================================================
// GlBufferManagerImplCreateException
//

struct GlBufferManagerImplCreateException :
	public Exception
{
	explicit GlBufferManagerImplCreateException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR_INIT] "} + message}
	{
	}
}; // GlBufferManagerImplCreateException

//
// GlBufferManagerImplCreateException
// ==========================================================================


// ==========================================================================
// GlBufferManagerImpl
//

class GlBufferManagerImpl final :
	public GlBufferManager
{
public:
	GlBufferManagerImpl(
		const GlContextPtr gl_context,
		const GlVaoManagerPtr gl_vao_manager);

	~GlBufferManagerImpl() override;


	GlContextPtr get_context() const noexcept override;


	Renderer3dBufferUPtr buffer_create(
		const Renderer3dBufferCreateParam& param) override;

	void buffer_notify_destroy(
		const Renderer3dBufferPtr buffer) noexcept override;


	bool buffer_set_current(
		const Renderer3dBufferKind buffer_kind,
		const Renderer3dBufferPtr index_buffer) override;


private:
	const GlContextPtr gl_context_;
	const GlVaoManagerPtr gl_vao_manager_;

	Renderer3dBufferPtr vertex_buffer_current_;
}; // GlBufferManagerImpl

using GlBufferManagerImplPtr = GlBufferManagerImpl*;
using GlBufferManagerImplUPtr = std::unique_ptr<GlBufferManagerImpl>;

//
// GlBufferManagerImpl
// ==========================================================================


// ==========================================================================
// GlBufferManagerImpl
//

GlBufferManagerImpl::GlBufferManagerImpl(
	const GlContextPtr gl_context,
	const GlVaoManagerPtr gl_vao_manager)
	:
	gl_context_{gl_context},
	gl_vao_manager_{gl_vao_manager},
	vertex_buffer_current_{}
{
	if (!gl_context_)
	{
		throw GlBufferManagerImplCreateException{"Null context."};
	}

	if (!gl_vao_manager_)
	{
		throw GlBufferManagerImplCreateException{"Null VAO manager."};
	}
}

GlBufferManagerImpl::~GlBufferManagerImpl() = default;

GlContextPtr GlBufferManagerImpl::get_context() const noexcept
{
	return gl_context_;
}

Renderer3dBufferUPtr GlBufferManagerImpl::buffer_create(
	const Renderer3dBufferCreateParam& param)
{
	return GlBufferFactory::create(this, param);
}

void GlBufferManagerImpl::buffer_notify_destroy(
	const Renderer3dBufferPtr buffer) noexcept
{
	if (vertex_buffer_current_ == buffer)
	{
		vertex_buffer_current_ = nullptr;
	}
}

bool GlBufferManagerImpl::buffer_set_current(
	const Renderer3dBufferKind buffer_kind,
	const Renderer3dBufferPtr buffer)
{
	Renderer3dBufferPtr* buffer_current_ptr = nullptr;

	switch (buffer_kind)
	{
		case Renderer3dBufferKind::index:
			return gl_vao_manager_->set_current_index_buffer(buffer);

		case Renderer3dBufferKind::vertex:
			buffer_current_ptr = &vertex_buffer_current_;

			break;

		default:
			throw GlBufferManagerImplException{"Unsupported buffer kind."};
	}

	if (!buffer_current_ptr)
	{
		throw GlBufferManagerImplException{"Null current buffer."};
	}

	if (*buffer_current_ptr == buffer)
	{
		return false;
	}

	*buffer_current_ptr = buffer;

	return true;
}

//
// GlBufferManagerImpl
// ==========================================================================


// ==========================================================================
// GlBufferManagerFactory
//

GlBufferManagerUPtr GlBufferManagerFactory::create(
	const GlContextPtr gl_context,
	const GlVaoManagerPtr gl_vao_manager)
{
	return std::make_unique<GlBufferManagerImpl>(gl_context, gl_vao_manager);
}

//
// GlBufferManagerFactory
// ==========================================================================


} // detail
} // bstone
