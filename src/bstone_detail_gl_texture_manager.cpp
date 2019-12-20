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
#include "bstone_renderer_3d.h"

#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_sampler_manager.h"
#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_texture_2d.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlTextureManager
//

GlTextureManager::GlTextureManager() = default;

GlTextureManager::~GlTextureManager() = default;

//
// GlTextureManager
// ==========================================================================


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

	bool set_current(
		const Renderer3dTexture2dPtr texture_2d) override;

	void update_current_sampler_state(
		const Renderer3dSamplerState& sampler_state) override;


private:
	const GlContextPtr gl_context_;

	GlTexture2dPtr texture_2d_current_;


	void initialize();

	void set();
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
	texture_2d_current_{}
{
	initialize();
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
	if (texture_2d_current_ == texture_2d)
	{
		texture_2d_current_ = nullptr;
	}
}

void GlTextureManagerImpl::set(
	const Renderer3dTexture2dPtr texture_2d)
{
	if (!set_current(texture_2d))
	{
		return;
	}

	if (!texture_2d_current_)
	{
		return;
	}

	texture_2d_current_->bind();

	const auto& device_features = gl_context_->get_device_features();

	if (!device_features.sampler_is_available_)
	{
		const auto sampler_manager = gl_context_->sampler_get_manager();
		const auto& sampler_state = sampler_manager->get_current_state();

		texture_2d_current_->update_sampler_state(sampler_state);
	}
}

bool GlTextureManagerImpl::set_current(
	const Renderer3dTexture2dPtr texture_2d)
{
	if (texture_2d_current_ == texture_2d)
	{
		return false;
	}

	texture_2d_current_ = static_cast<GlTexture2dPtr>(texture_2d);

	return true;
}

void GlTextureManagerImpl::update_current_sampler_state(
	const Renderer3dSamplerState& sampler_state)
{
	if (!texture_2d_current_)
	{
		return;
	}

	texture_2d_current_->update_sampler_state(sampler_state);
}

void GlTextureManagerImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	GlRenderer3dUtils::texture_2d_unbind();
}

void GlTextureManagerImpl::set()
{
	if (texture_2d_current_)
	{
		texture_2d_current_->bind();
	}
	else
	{
		GlRenderer3dUtils::texture_2d_unbind();
	}
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
