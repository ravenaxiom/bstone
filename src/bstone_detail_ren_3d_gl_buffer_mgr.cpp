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
#include "bstone_detail_ren_3d_gl_buffer_mgr.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_buffer.h"
#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_vao_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGlBufferMgrImplException
//

struct Ren3dGlBufferMgrImplException :
	public Exception
{
	explicit Ren3dGlBufferMgrImplException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR] "} + message}
	{
	}
}; // Ren3dGlBufferMgrImplException

//
// Ren3dGlBufferMgrImplException
// ==========================================================================


// ==========================================================================
// Ren3dGlBufferMgrImplCreateException
//

struct Ren3dGlBufferMgrImplCreateException :
	public Exception
{
	explicit Ren3dGlBufferMgrImplCreateException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_MGR_INIT] "} + message}
	{
	}
}; // Ren3dGlBufferMgrImplCreateException

//
// Ren3dGlBufferMgrImplCreateException
// ==========================================================================


// ==========================================================================
// Ren3dGlBufferMgrImpl
//

class Ren3dGlBufferMgrImpl final :
	public Ren3dGlBufferMgr
{
public:
	Ren3dGlBufferMgrImpl(
		const Ren3dGlContextPtr gl_context,
		const Ren3dGlVaoMgrPtr gl_vao_manager);

	~Ren3dGlBufferMgrImpl() override;


	Ren3dGlContextPtr get_context() const noexcept override;


	Ren3dBufferUPtr buffer_create(
		const Ren3dBufferCreateParam& param) override;

	void buffer_notify_destroy(
		const Ren3dBufferPtr buffer) noexcept override;


	bool buffer_set_current(
		const Ren3dBufferKind buffer_kind,
		const Ren3dBufferPtr index_buffer) override;


private:
	const Ren3dGlContextPtr gl_context_;
	const Ren3dGlVaoMgrPtr gl_vao_manager_;

	Ren3dBufferPtr vertex_buffer_current_;
}; // Ren3dGlBufferMgrImpl

using Ren3dGlBufferMgrImplPtr = Ren3dGlBufferMgrImpl*;
using Ren3dGlBufferMgrImplUPtr = std::unique_ptr<Ren3dGlBufferMgrImpl>;

//
// Ren3dGlBufferMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlBufferMgrImpl
//

Ren3dGlBufferMgrImpl::Ren3dGlBufferMgrImpl(
	const Ren3dGlContextPtr gl_context,
	const Ren3dGlVaoMgrPtr gl_vao_manager)
	:
	gl_context_{gl_context},
	gl_vao_manager_{gl_vao_manager},
	vertex_buffer_current_{}
{
	if (!gl_context_)
	{
		throw Ren3dGlBufferMgrImplCreateException{"Null context."};
	}

	if (!gl_vao_manager_)
	{
		throw Ren3dGlBufferMgrImplCreateException{"Null VAO manager."};
	}
}

Ren3dGlBufferMgrImpl::~Ren3dGlBufferMgrImpl() = default;

Ren3dGlContextPtr Ren3dGlBufferMgrImpl::get_context() const noexcept
{
	return gl_context_;
}

Ren3dBufferUPtr Ren3dGlBufferMgrImpl::buffer_create(
	const Ren3dBufferCreateParam& param)
{
	return Ren3dGlBufferFactory::create(this, param);
}

void Ren3dGlBufferMgrImpl::buffer_notify_destroy(
	const Ren3dBufferPtr buffer) noexcept
{
	if (vertex_buffer_current_ == buffer)
	{
		vertex_buffer_current_ = nullptr;
	}
}

bool Ren3dGlBufferMgrImpl::buffer_set_current(
	const Ren3dBufferKind buffer_kind,
	const Ren3dBufferPtr buffer)
{
	Ren3dBufferPtr* buffer_current_ptr = nullptr;

	switch (buffer_kind)
	{
		case Ren3dBufferKind::index:
			return gl_vao_manager_->set_current_index_buffer(buffer);

		case Ren3dBufferKind::vertex:
			buffer_current_ptr = &vertex_buffer_current_;

			break;

		default:
			throw Ren3dGlBufferMgrImplException{"Unsupported buffer kind."};
	}

	if (!buffer_current_ptr)
	{
		throw Ren3dGlBufferMgrImplException{"Null current buffer."};
	}

	if (*buffer_current_ptr == buffer)
	{
		return false;
	}

	*buffer_current_ptr = buffer;

	return true;
}

//
// Ren3dGlBufferMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlBufferMgrFactory
//

Ren3dGlBufferMgrUPtr Ren3dGlBufferMgrFactory::create(
	const Ren3dGlContextPtr gl_context,
	const Ren3dGlVaoMgrPtr gl_vao_manager)
{
	return std::make_unique<Ren3dGlBufferMgrImpl>(gl_context, gl_vao_manager);
}

//
// Ren3dGlBufferMgrFactory
// ==========================================================================


} // detail
} // bstone
