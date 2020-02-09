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
// OpenGL sampler object manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_sampler_manager.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"

#include "bstone_detail_gl_sampler.h"
#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_texture_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlSamplerManagerImpl
//

class GlSamplerManagerImpl final :
	public GlSamplerManager
{
public:
	GlSamplerManagerImpl(
		const GlContextPtr gl_context);

	~GlSamplerManagerImpl() override;


	Renderer3dSamplerUPtr create(
		const Renderer3dSamplerCreateParam& param) override;

	void notify_destroy(
		const Renderer3dSamplerPtr sampler) noexcept override;

	void set(
		const Renderer3dSamplerPtr sampler) override;

	const Renderer3dSamplerState& get_current_state() const noexcept override;


private:
	const GlContextPtr gl_context_;
	const Renderer3dDeviceFeatures& device_features_;

	GlSamplerUPtr sampler_default_;
	GlSamplerPtr sampler_current_;


	void initialize_default_sampler();

	void set();
}; // GlSamplerManagerImpl

using GlSamplerManagerImplPtr = GlSamplerManagerImpl*;
using GlSamplerManagerImplUPtr = std::unique_ptr<GlSamplerManagerImpl>;

//
// GlSamplerManagerImpl
// ==========================================================================


// ==========================================================================
// GlSamplerManagerImpl
//

GlSamplerManagerImpl::GlSamplerManagerImpl(
	const GlContextPtr gl_context)
	:
	gl_context_{gl_context},
	device_features_{gl_context_->get_device_features()},
	sampler_default_{},
	sampler_current_{}
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	initialize_default_sampler();

	sampler_current_ = sampler_default_.get();
}

GlSamplerManagerImpl::~GlSamplerManagerImpl() = default;

Renderer3dSamplerUPtr GlSamplerManagerImpl::create(
	const Renderer3dSamplerCreateParam& param)
{
	return GlSamplerFactory::create(gl_context_, param);
}

void GlSamplerManagerImpl::notify_destroy(
	const Renderer3dSamplerPtr sampler) noexcept
{
	if (sampler_current_ == sampler)
	{
		if (device_features_.sampler_is_available_)
		{
			sampler_current_ = nullptr;
		}
		else
		{
			sampler_current_ = sampler_default_.get();
		}
	}
}

void GlSamplerManagerImpl::set(
	const Renderer3dSamplerPtr sampler)
{
	if (!sampler)
	{
		throw Exception{"Null sampler."};
	}

	if (sampler_current_ == sampler)
	{
		return;
	}

	sampler_current_ = static_cast<GlSamplerPtr>(sampler);
	set();
}

const Renderer3dSamplerState& GlSamplerManagerImpl::get_current_state() const noexcept
{
	return sampler_current_->get_state();
}

void GlSamplerManagerImpl::initialize_default_sampler()
{
	const auto param = Renderer3dSamplerCreateParam{};

	sampler_default_ = GlSamplerFactory::create(gl_context_, param);
}

void GlSamplerManagerImpl::set()
{
	if (device_features_.sampler_is_available_)
	{
		sampler_current_->set();
	}
	else
	{
		const auto texture_manager = gl_context_->texture_get_manager();

		texture_manager->update_current_sampler_state(get_current_state());
	}
}

//
// GlSamplerManagerImpl
// ==========================================================================


// ==========================================================================
// GlSamplerManagerFactory
//

GlSamplerManagerUPtr GlSamplerManagerFactory::create(
	const GlContextPtr gl_context)
{
	return std::make_unique<GlSamplerManagerImpl>(gl_context);
}

//
// GlSamplerManagerFactory
// ==========================================================================


} // detail
} // bstone
