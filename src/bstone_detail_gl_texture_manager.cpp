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
#include "bstone_detail_gl_texture_manager.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"
#include "bstone_renderer_3d_texture_2d.h"

#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_error.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_texture_2d.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlTextureManagerImpl
//

class GlTextureManagerImpl final :
	public GlTextureManager
{
public:
	GlTextureManagerImpl(
		const GlContextPtr gl_context);

	~GlTextureManagerImpl() override;


	GlContextPtr get_gl_context() const noexcept override;


	Renderer3dTexture2dUPtr create(
		const Renderer3dTexture2dCreateParam& param) override;


	void notify_destroy(
		const Renderer3dTexture2dPtr texture_2d) noexcept override;


	void set(
		const Renderer3dTexture2dPtr texture_2d) override;


	Renderer3dTexture2dPtr get_active() const noexcept override;

	void set_active(
		const Renderer3dTexture2dPtr texture_2d) override;


	Renderer3dTexture2dPtr get_current() const noexcept override;

	void set_current(
		const Renderer3dTexture2dPtr texture_2d) override;


	void set_to_current() override;


	void update_current_sampler_state(
		const Renderer3dSamplerState& sampler_state) override;


private:
	const GlContextPtr gl_context_;

	GlTexture2dPtr texture_2d_active_;
	GlTexture2dPtr texture_2d_current_;


	void unbind();
}; // GlTextureManagerImpl

using GlTextureManagerImplPtr = GlTextureManagerImpl*;
using GlTextureManagerImplUPtr = std::unique_ptr<GlTextureManagerImpl>;

//
// GlTextureManagerImpl
// ==========================================================================


// ==========================================================================
// GlTextureManagerImpl
//

GlTextureManagerImpl::GlTextureManagerImpl(
	const GlContextPtr gl_context)
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

GlTextureManagerImpl::~GlTextureManagerImpl() = default;

GlContextPtr GlTextureManagerImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

Renderer3dTexture2dUPtr GlTextureManagerImpl::create(
	const Renderer3dTexture2dCreateParam& param)
{
	return GlTexture2dFactory::create(this, param);
}

void GlTextureManagerImpl::notify_destroy(
	const Renderer3dTexture2dPtr texture_2d) noexcept
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

void GlTextureManagerImpl::set(
	const Renderer3dTexture2dPtr texture_2d)
{
	if (texture_2d == texture_2d_active_)
	{
		return;
	}

	if (texture_2d)
	{
		static_cast<GlTexture2dPtr>(texture_2d)->set();
	}
	else
	{
		unbind();

		texture_2d_active_ = nullptr;
	}
}

Renderer3dTexture2dPtr GlTextureManagerImpl::get_active() const noexcept
{
	return texture_2d_active_;
}

void GlTextureManagerImpl::set_active(
	const Renderer3dTexture2dPtr texture_2d)
{
	texture_2d_active_ = static_cast<GlTexture2dPtr>(texture_2d);
}

Renderer3dTexture2dPtr GlTextureManagerImpl::get_current() const noexcept
{
	return texture_2d_current_;
}

void GlTextureManagerImpl::set_current(
	const Renderer3dTexture2dPtr texture_2d)
{
	texture_2d_current_ = static_cast<GlTexture2dPtr>(texture_2d);
}

void GlTextureManagerImpl::set_to_current()
{
	set(texture_2d_current_);
}

void GlTextureManagerImpl::update_current_sampler_state(
	const Renderer3dSamplerState& sampler_state)
{
	if (!texture_2d_active_)
	{
		return;
	}

	texture_2d_active_->update_sampler_state(sampler_state);
}

void GlTextureManagerImpl::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	GlError::ensure_debug();
}

//
// GlTextureManagerImpl
// ==========================================================================


// ==========================================================================
// GlTextureManagerFactory
//

GlTextureManagerUPtr GlTextureManagerFactory::create(
	const GlContextPtr gl_context)
{
	return std::make_unique<GlTextureManagerImpl>(gl_context);
}

//
// GlTextureManagerFactory
// ==========================================================================


} // detail
} // bstone
