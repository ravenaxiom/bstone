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
// GlBufferMgrImplException
//

struct GlBufferMgrImplException :
	public Exception
{
	explicit GlBufferMgrImplException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR] "} + message}
	{
	}
}; // GlBufferMgrImplException

//
// GlBufferMgrImplException
// ==========================================================================


// ==========================================================================
// GlBufferMgrImplCreateException
//

struct GlBufferMgrImplCreateException :
	public Exception
{
	explicit GlBufferMgrImplCreateException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR_INIT] "} + message}
	{
	}
}; // GlBufferMgrImplCreateException

//
// GlBufferMgrImplCreateException
// ==========================================================================


// ==========================================================================
// R3dGlBufferMgrImpl
//

class R3dGlBufferMgrImpl final :
	public R3dGlBufferMgr
{
public:
	R3dGlBufferMgrImpl(
		const R3dGlContextPtr gl_context,
		const R3dGlVaoMgrPtr gl_vao_manager);

	~R3dGlBufferMgrImpl() override;


	R3dGlContextPtr get_context() const noexcept override;


	R3dBufferUPtr buffer_create(
		const R3dBufferCreateParam& param) override;

	void buffer_notify_destroy(
		const R3dBufferPtr buffer) noexcept override;


	bool buffer_set_current(
		const R3dBufferKind buffer_kind,
		const R3dBufferPtr index_buffer) override;


private:
	const R3dGlContextPtr gl_context_;
	const R3dGlVaoMgrPtr gl_vao_manager_;

	R3dBufferPtr vertex_buffer_current_;
}; // R3dGlBufferMgrImpl

using R3dGlBufferMgrImplPtr = R3dGlBufferMgrImpl*;
using R3dGlBufferMgrImplUPtr = std::unique_ptr<R3dGlBufferMgrImpl>;

//
// R3dGlBufferMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlBufferMgrImpl
//

R3dGlBufferMgrImpl::R3dGlBufferMgrImpl(
	const R3dGlContextPtr gl_context,
	const R3dGlVaoMgrPtr gl_vao_manager)
	:
	gl_context_{gl_context},
	gl_vao_manager_{gl_vao_manager},
	vertex_buffer_current_{}
{
	if (!gl_context_)
	{
		throw GlBufferMgrImplCreateException{"Null context."};
	}

	if (!gl_vao_manager_)
	{
		throw GlBufferMgrImplCreateException{"Null VAO manager."};
	}
}

R3dGlBufferMgrImpl::~R3dGlBufferMgrImpl() = default;

R3dGlContextPtr R3dGlBufferMgrImpl::get_context() const noexcept
{
	return gl_context_;
}

R3dBufferUPtr R3dGlBufferMgrImpl::buffer_create(
	const R3dBufferCreateParam& param)
{
	return R3dGlBufferFactory::create(this, param);
}

void R3dGlBufferMgrImpl::buffer_notify_destroy(
	const R3dBufferPtr buffer) noexcept
{
	if (vertex_buffer_current_ == buffer)
	{
		vertex_buffer_current_ = nullptr;
	}
}

bool R3dGlBufferMgrImpl::buffer_set_current(
	const R3dBufferKind buffer_kind,
	const R3dBufferPtr buffer)
{
	R3dBufferPtr* buffer_current_ptr = nullptr;

	switch (buffer_kind)
	{
		case R3dBufferKind::index:
			return gl_vao_manager_->set_current_index_buffer(buffer);

		case R3dBufferKind::vertex:
			buffer_current_ptr = &vertex_buffer_current_;

			break;

		default:
			throw GlBufferMgrImplException{"Unsupported buffer kind."};
	}

	if (!buffer_current_ptr)
	{
		throw GlBufferMgrImplException{"Null current buffer."};
	}

	if (*buffer_current_ptr == buffer)
	{
		return false;
	}

	*buffer_current_ptr = buffer;

	return true;
}

//
// R3dGlBufferMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlBufferMgrFactory
//

R3dGlBufferMgrUPtr R3dGlBufferMgrFactory::create(
	const R3dGlContextPtr gl_context,
	const R3dGlVaoMgrPtr gl_vao_manager)
{
	return std::make_unique<R3dGlBufferMgrImpl>(gl_context, gl_vao_manager);
}

//
// R3dGlBufferMgrFactory
// ==========================================================================


} // detail
} // bstone
