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
// OpenGL texture object manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_texture_mgr.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"
#include "bstone_r3d_texture_2d.h"

#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_texture_2d.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlTextureMgrImpl
//

class R3dGlTextureMgrImpl final :
	public R3dGlTextureMgr
{
public:
	R3dGlTextureMgrImpl(
		const R3dGlContextPtr gl_context);

	~R3dGlTextureMgrImpl() override;


	R3dGlContextPtr get_gl_context() const noexcept override;


	R3dTexture2dUPtr create(
		const R3dTexture2dCreateParam& param) override;


	void notify_destroy(
		const R3dTexture2dPtr texture_2d) noexcept override;


	void set(
		const R3dTexture2dPtr texture_2d) override;


	R3dTexture2dPtr get_active() const noexcept override;

	void set_active(
		const R3dTexture2dPtr texture_2d) override;


	R3dTexture2dPtr get_current() const noexcept override;

	void set_current(
		const R3dTexture2dPtr texture_2d) override;


	void set_to_current() override;


	void update_current_sampler_state(
		const R3dSamplerState& sampler_state) override;


private:
	const R3dGlContextPtr gl_context_;

	R3dGlTexture2dPtr texture_2d_active_;
	R3dGlTexture2dPtr texture_2d_current_;


	void unbind();
}; // R3dGlTextureMgrImpl

using R3dGlTextureMgrImplPtr = R3dGlTextureMgrImpl*;
using R3dGlTextureMgrImplUPtr = std::unique_ptr<R3dGlTextureMgrImpl>;

//
// R3dGlTextureMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlTextureMgrImpl
//

R3dGlTextureMgrImpl::R3dGlTextureMgrImpl(
	const R3dGlContextPtr gl_context)
	:
	gl_context_{gl_context},
	texture_2d_active_{},
	texture_2d_current_{}
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	unbind();
}

R3dGlTextureMgrImpl::~R3dGlTextureMgrImpl() = default;

R3dGlContextPtr R3dGlTextureMgrImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

R3dTexture2dUPtr R3dGlTextureMgrImpl::create(
	const R3dTexture2dCreateParam& param)
{
	return R3dGlTexture2dFactory::create(this, param);
}

void R3dGlTextureMgrImpl::notify_destroy(
	const R3dTexture2dPtr texture_2d) noexcept
{
	if (texture_2d == texture_2d_active_)
	{
		texture_2d_active_ = nullptr;
	}

	if (texture_2d == texture_2d_current_)
	{
		texture_2d_current_ = nullptr;
	}
}

void R3dGlTextureMgrImpl::set(
	const R3dTexture2dPtr texture_2d)
{
	if (texture_2d == texture_2d_active_)
	{
		return;
	}

	if (texture_2d)
	{
		static_cast<R3dGlTexture2dPtr>(texture_2d)->set();
	}
	else
	{
		unbind();

		texture_2d_active_ = nullptr;
	}
}

R3dTexture2dPtr R3dGlTextureMgrImpl::get_active() const noexcept
{
	return texture_2d_active_;
}

void R3dGlTextureMgrImpl::set_active(
	const R3dTexture2dPtr texture_2d)
{
	texture_2d_active_ = static_cast<R3dGlTexture2dPtr>(texture_2d);
}

R3dTexture2dPtr R3dGlTextureMgrImpl::get_current() const noexcept
{
	return texture_2d_current_;
}

void R3dGlTextureMgrImpl::set_current(
	const R3dTexture2dPtr texture_2d)
{
	texture_2d_current_ = static_cast<R3dGlTexture2dPtr>(texture_2d);
}

void R3dGlTextureMgrImpl::set_to_current()
{
	set(texture_2d_current_);
}

void R3dGlTextureMgrImpl::update_current_sampler_state(
	const R3dSamplerState& sampler_state)
{
	if (!texture_2d_active_)
	{
		return;
	}

	texture_2d_active_->update_sampler_state(sampler_state);
}

void R3dGlTextureMgrImpl::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	R3dGlError::ensure_debug();
}

//
// R3dGlTextureMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlTextureMgrFactory
//

R3dGlTextureMgrUPtr R3dGlTextureMgrFactory::create(
	const R3dGlContextPtr gl_context)
{
	return std::make_unique<R3dGlTextureMgrImpl>(gl_context);
}

//
// R3dGlTextureMgrFactory
// ==========================================================================


} // detail
} // bstone
