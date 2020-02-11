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
#include "bstone_detail_r3d_gl_sampler_mgr.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_sampler.h"
#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_texture_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlSamplerMgrImpl
//

class R3dGlSamplerMgrImpl final :
	public R3dGlSamplerMgr
{
public:
	R3dGlSamplerMgrImpl(
		const R3dGlContextPtr gl_context);

	~R3dGlSamplerMgrImpl() override;


	R3dSamplerUPtr create(
		const R3dSamplerCreateParam& param) override;

	void notify_destroy(
		const R3dSamplerPtr sampler) noexcept override;

	void set(
		const R3dSamplerPtr sampler) override;

	const R3dSamplerState& get_current_state() const noexcept override;


private:
	const R3dGlContextPtr gl_context_;
	const R3dDeviceFeatures& device_features_;

	R3dGlSamplerUPtr sampler_default_;
	R3dGlSamplerPtr sampler_current_;


	void initialize_default_sampler();

	void set();
}; // R3dGlSamplerMgrImpl

using R3dGlSamplerMgrImplPtr = R3dGlSamplerMgrImpl*;
using R3dGlSamplerMgrImplUPtr = std::unique_ptr<R3dGlSamplerMgrImpl>;

//
// R3dGlSamplerMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlSamplerMgrImpl
//

R3dGlSamplerMgrImpl::R3dGlSamplerMgrImpl(
	const R3dGlContextPtr gl_context)
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

R3dGlSamplerMgrImpl::~R3dGlSamplerMgrImpl() = default;

R3dSamplerUPtr R3dGlSamplerMgrImpl::create(
	const R3dSamplerCreateParam& param)
{
	return R3dGlSamplerFactory::create(gl_context_, param);
}

void R3dGlSamplerMgrImpl::notify_destroy(
	const R3dSamplerPtr sampler) noexcept
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

void R3dGlSamplerMgrImpl::set(
	const R3dSamplerPtr sampler)
{
	if (!sampler)
	{
		throw Exception{"Null sampler."};
	}

	if (sampler_current_ == sampler)
	{
		return;
	}

	sampler_current_ = static_cast<R3dGlSamplerPtr>(sampler);
	set();
}

const R3dSamplerState& R3dGlSamplerMgrImpl::get_current_state() const noexcept
{
	return sampler_current_->get_state();
}

void R3dGlSamplerMgrImpl::initialize_default_sampler()
{
	const auto param = R3dSamplerCreateParam{};

	sampler_default_ = R3dGlSamplerFactory::create(gl_context_, param);
}

void R3dGlSamplerMgrImpl::set()
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
// R3dGlSamplerMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlSamplerMgrFactory
//

R3dGlSamplerMgrUPtr R3dGlSamplerMgrFactory::create(
	const R3dGlContextPtr gl_context)
{
	return std::make_unique<R3dGlSamplerMgrImpl>(gl_context);
}

//
// R3dGlSamplerMgrFactory
// ==========================================================================


} // detail
} // bstone
