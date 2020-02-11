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
// Ren3dGlTextureMgrImpl
//

class Ren3dGlTextureMgrImpl final :
	public Ren3dGlTextureMgr
{
public:
	Ren3dGlTextureMgrImpl(
		const Ren3dGlContextPtr gl_context);

	~Ren3dGlTextureMgrImpl() override;


	Ren3dGlContextPtr get_gl_context() const noexcept override;


	Ren3dTexture2dUPtr create(
		const Ren3dTexture2dCreateParam& param) override;


	void notify_destroy(
		const Ren3dTexture2dPtr texture_2d) noexcept override;


	void set(
		const Ren3dTexture2dPtr texture_2d) override;


	Ren3dTexture2dPtr get_active() const noexcept override;

	void set_active(
		const Ren3dTexture2dPtr texture_2d) override;


	Ren3dTexture2dPtr get_current() const noexcept override;

	void set_current(
		const Ren3dTexture2dPtr texture_2d) override;


	void set_to_current() override;


	void update_current_sampler_state(
		const Ren3dSamplerState& sampler_state) override;


private:
	const Ren3dGlContextPtr gl_context_;

	Ren3dGlTexture2dPtr texture_2d_active_;
	Ren3dGlTexture2dPtr texture_2d_current_;


	void unbind();
}; // Ren3dGlTextureMgrImpl

using Ren3dGlTextureMgrImplPtr = Ren3dGlTextureMgrImpl*;
using Ren3dGlTextureMgrImplUPtr = std::unique_ptr<Ren3dGlTextureMgrImpl>;

//
// Ren3dGlTextureMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlTextureMgrImpl
//

Ren3dGlTextureMgrImpl::Ren3dGlTextureMgrImpl(
	const Ren3dGlContextPtr gl_context)
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

Ren3dGlTextureMgrImpl::~Ren3dGlTextureMgrImpl() = default;

Ren3dGlContextPtr Ren3dGlTextureMgrImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

Ren3dTexture2dUPtr Ren3dGlTextureMgrImpl::create(
	const Ren3dTexture2dCreateParam& param)
{
	return Ren3dGlTexture2dFactory::create(this, param);
}

void Ren3dGlTextureMgrImpl::notify_destroy(
	const Ren3dTexture2dPtr texture_2d) noexcept
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

void Ren3dGlTextureMgrImpl::set(
	const Ren3dTexture2dPtr texture_2d)
{
	if (texture_2d == texture_2d_active_)
	{
		return;
	}

	if (texture_2d)
	{
		static_cast<Ren3dGlTexture2dPtr>(texture_2d)->set();
	}
	else
	{
		unbind();

		texture_2d_active_ = nullptr;
	}
}

Ren3dTexture2dPtr Ren3dGlTextureMgrImpl::get_active() const noexcept
{
	return texture_2d_active_;
}

void Ren3dGlTextureMgrImpl::set_active(
	const Ren3dTexture2dPtr texture_2d)
{
	texture_2d_active_ = static_cast<Ren3dGlTexture2dPtr>(texture_2d);
}

Ren3dTexture2dPtr Ren3dGlTextureMgrImpl::get_current() const noexcept
{
	return texture_2d_current_;
}

void Ren3dGlTextureMgrImpl::set_current(
	const Ren3dTexture2dPtr texture_2d)
{
	texture_2d_current_ = static_cast<Ren3dGlTexture2dPtr>(texture_2d);
}

void Ren3dGlTextureMgrImpl::set_to_current()
{
	set(texture_2d_current_);
}

void Ren3dGlTextureMgrImpl::update_current_sampler_state(
	const Ren3dSamplerState& sampler_state)
{
	if (!texture_2d_active_)
	{
		return;
	}

	texture_2d_active_->update_sampler_state(sampler_state);
}

void Ren3dGlTextureMgrImpl::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	Ren3dGlError::ensure_debug();
}

//
// Ren3dGlTextureMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlTextureMgrFactory
//

Ren3dGlTextureMgrUPtr Ren3dGlTextureMgrFactory::create(
	const Ren3dGlContextPtr gl_context)
{
	return std::make_unique<Ren3dGlTextureMgrImpl>(gl_context);
}

//
// Ren3dGlTextureMgrFactory
// ==========================================================================


} // detail
} // bstone
