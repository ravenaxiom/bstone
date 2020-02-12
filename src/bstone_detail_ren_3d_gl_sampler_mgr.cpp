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
#include "bstone_detail_ren_3d_gl_sampler_mgr.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_sampler.h"
#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_texture_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGlSamplerMgrImpl
//

class Ren3dGlSamplerMgrImpl final :
	public Ren3dGlSamplerMgr
{
public:
	Ren3dGlSamplerMgrImpl(
		const Ren3dGlContextPtr gl_context);

	~Ren3dGlSamplerMgrImpl() override;


	Ren3dSamplerUPtr create(
		const Ren3dCreateSamplerParam& param) override;

	void notify_destroy(
		const Ren3dSamplerPtr sampler) noexcept override;

	void set(
		const Ren3dSamplerPtr sampler) override;

	const Ren3dSamplerState& get_current_state() const noexcept override;


private:
	const Ren3dGlContextPtr gl_context_;
	const Ren3dDeviceFeatures& device_features_;

	Ren3dGlSamplerUPtr sampler_default_;
	Ren3dGlSamplerPtr sampler_current_;


	void initialize_default_sampler();

	void set();
}; // Ren3dGlSamplerMgrImpl

using Ren3dGlSamplerMgrImplPtr = Ren3dGlSamplerMgrImpl*;
using Ren3dGlSamplerMgrImplUPtr = std::unique_ptr<Ren3dGlSamplerMgrImpl>;

//
// Ren3dGlSamplerMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlSamplerMgrImpl
//

Ren3dGlSamplerMgrImpl::Ren3dGlSamplerMgrImpl(
	const Ren3dGlContextPtr gl_context)
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

Ren3dGlSamplerMgrImpl::~Ren3dGlSamplerMgrImpl() = default;

Ren3dSamplerUPtr Ren3dGlSamplerMgrImpl::create(
	const Ren3dCreateSamplerParam& param)
{
	return Ren3dGlSamplerFactory::create(gl_context_, param);
}

void Ren3dGlSamplerMgrImpl::notify_destroy(
	const Ren3dSamplerPtr sampler) noexcept
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

void Ren3dGlSamplerMgrImpl::set(
	const Ren3dSamplerPtr sampler)
{
	if (!sampler)
	{
		throw Exception{"Null sampler."};
	}

	if (sampler_current_ == sampler)
	{
		return;
	}

	sampler_current_ = static_cast<Ren3dGlSamplerPtr>(sampler);
	set();
}

const Ren3dSamplerState& Ren3dGlSamplerMgrImpl::get_current_state() const noexcept
{
	return sampler_current_->get_state();
}

void Ren3dGlSamplerMgrImpl::initialize_default_sampler()
{
	const auto param = Ren3dCreateSamplerParam{};

	sampler_default_ = Ren3dGlSamplerFactory::create(gl_context_, param);
}

void Ren3dGlSamplerMgrImpl::set()
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
// Ren3dGlSamplerMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlSamplerMgrFactory
//

Ren3dGlSamplerMgrUPtr Ren3dGlSamplerMgrFactory::create(
	const Ren3dGlContextPtr gl_context)
{
	return std::make_unique<Ren3dGlSamplerMgrImpl>(gl_context);
}

//
// Ren3dGlSamplerMgrFactory
// ==========================================================================


} // detail
} // bstone
