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
// OpenGL sampler (implementation).
//
// !!! Internal usage only !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_ren_3d_gl_sampler.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_ren_3d_gl_api.h"
#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_error.h"
#include "bstone_detail_ren_3d_gl_utils.h"
#include "bstone_detail_ren_3d_gl_texture_mgr.h"


namespace bstone
{
namespace detail
{


// =========================================================================
// GlSamplerImpl
//

class GlSamplerImpl final :
	public Ren3dGlSampler
{
public:
	GlSamplerImpl(
		Ren3dGlContextPtr gl_context,
		const Ren3dSamplerCreateParam& param);

	GlSamplerImpl(
		const Ren3dGlSampler& rhs) = delete;

	~GlSamplerImpl() override;


	void set() override;

	void update(
		const Ren3dSamplerUpdateParam& param) override;

	const Ren3dSamplerState& get_state() const noexcept override;


private:
	static void sampler_deleter(
		const GLuint& resource) noexcept;

	using SamplerResource = bstone::UniqueResource<GLuint, sampler_deleter>;


	Ren3dGlContextPtr gl_context_;

	Ren3dSamplerState state_;
	SamplerResource gl_resource_;


	void set_mag_filter();

	void set_min_filter();

	void set_address_mode(
		const Ren3dTextureAxis texture_axis,
		const Ren3dAddressMode address_mode);

	void set_address_mode_u();

	void set_address_mode_v();

	void set_anisotropy();

	void set_initial_state();
}; // GlSamplerImpl

using GlSamplerImplPtr = GlSamplerImpl*;
using GlSamplerImplUPtr = std::unique_ptr<GlSamplerImpl>;

//
// GlSamplerImpl
// =========================================================================


// =========================================================================
// GlSamplerImpl
//

GlSamplerImpl::GlSamplerImpl(
	Ren3dGlContextPtr gl_context,
	const Ren3dSamplerCreateParam& param)
	:
	gl_context_{gl_context},
	state_{}
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	const auto& device_features = gl_context_->get_device_features();
	const auto& gl_device_features = gl_context_->get_gl_device_features();

	state_ = param.state_;

	if (device_features.sampler_is_available_)
	{
		auto gl_name = GLuint{};

		if (gl_device_features.dsa_is_available_)
		{
			glCreateSamplers(1, &gl_name);
			Ren3dGlError::ensure_debug();
		}
		else
		{
			glGenSamplers(1, &gl_name);
			Ren3dGlError::ensure_debug();
		}

		if (gl_name == 0)
		{
			throw Exception{"Failed to create OpenGL sampler object."};
		}

		gl_resource_.reset(gl_name);
	}

	set_initial_state();
}

GlSamplerImpl::~GlSamplerImpl() = default;

void GlSamplerImpl::set()
{
	if (!gl_resource_)
	{
		return;
	}

	glBindSampler(0, gl_resource_);
	Ren3dGlError::ensure_debug();
}

void GlSamplerImpl::update(
	const Ren3dSamplerUpdateParam& param)
{
	auto is_modified = false;

	// Magnification filter.
	//
	auto is_mag_filter_modified = false;

	if (state_.mag_filter_ != param.state_.mag_filter_)
	{
		is_modified = true;
		is_mag_filter_modified = true;

		state_.mag_filter_ = param.state_.mag_filter_;
	}

	// Minification filter.
	//
	auto is_min_filter_modified = false;

	if (state_.min_filter_ != param.state_.min_filter_ ||
		state_.mipmap_mode_ != param.state_.mipmap_mode_)
	{
		is_modified = true;
		is_min_filter_modified = true;

		state_.min_filter_ = param.state_.min_filter_;
		state_.mipmap_mode_ = param.state_.mipmap_mode_;
	}

	// U-axis address mode.
	//
	auto is_address_mode_u = false;

	if (state_.address_mode_u_ != param.state_.address_mode_u_)
	{
		is_modified = true;
		is_address_mode_u = true;

		state_.address_mode_u_ = param.state_.address_mode_u_;
	}

	// V-axis address mode.
	//
	auto is_address_mode_v = false;

	if (state_.address_mode_v_ != param.state_.address_mode_v_)
	{
		is_modified = true;
		is_address_mode_v = true;

		state_.address_mode_v_ = param.state_.address_mode_v_;
	}

	// Anisotropy.
	//
	auto is_anisotropy = false;

	if (state_.anisotropy_ != param.state_.anisotropy_)
	{
		is_modified = true;
		is_anisotropy = true;

		state_.anisotropy_ = param.state_.anisotropy_;
	}


	// Modify.
	//
	if (is_modified)
	{
		if (gl_resource_)
		{
			if (is_mag_filter_modified)
			{
				set_mag_filter();
			}

			if (is_min_filter_modified)
			{
				set_min_filter();
			}

			if (is_address_mode_u)
			{
				set_address_mode_u();
			}

			if (is_address_mode_v)
			{
				set_address_mode_v();
			}

			if (is_anisotropy)
			{
				set_anisotropy();
			}
		}
		else
		{
			const auto texture_manager = gl_context_->texture_get_manager();

			texture_manager->update_current_sampler_state(state_);
		}
	}
}

const Ren3dSamplerState& GlSamplerImpl::get_state() const noexcept
{
	return state_;
}

void GlSamplerImpl::sampler_deleter(
	const GLuint& resource) noexcept
{
	glDeleteSamplers(1, &resource);
	Ren3dGlError::ensure_debug();
}

void GlSamplerImpl::set_mag_filter()
{
	const auto gl_mag_filter = Ren3dGlUtils::filter_get_mag(state_.mag_filter_);

	glSamplerParameteri(gl_resource_, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
	Ren3dGlError::ensure_debug();
}

void GlSamplerImpl::set_min_filter()
{
	const auto gl_min_filter = Ren3dGlUtils::filter_get_min(state_.min_filter_, state_.mipmap_mode_);

	glSamplerParameteri(gl_resource_, GL_TEXTURE_MIN_FILTER, gl_min_filter);
	Ren3dGlError::ensure_debug();
}

void GlSamplerImpl::set_address_mode(
	const Ren3dTextureAxis texture_axis,
	const Ren3dAddressMode address_mode)
{
	const auto gl_wrap_axis = Ren3dGlUtils::texture_wrap_get_axis(texture_axis);
	const auto gl_address_mode = Ren3dGlUtils::address_mode_get(address_mode);

	glSamplerParameteri(gl_resource_, gl_wrap_axis, gl_address_mode);
	Ren3dGlError::ensure_debug();
}

void GlSamplerImpl::set_address_mode_u()
{
	set_address_mode(Ren3dTextureAxis::u, state_.address_mode_u_);
}

void GlSamplerImpl::set_address_mode_v()
{
	set_address_mode(Ren3dTextureAxis::v, state_.address_mode_v_);
}

void GlSamplerImpl::set_anisotropy()
{
	Ren3dGlUtils::sampler_set_anisotropy(
		gl_resource_,
		gl_context_->get_device_features(),
		state_.anisotropy_
	);
}

void GlSamplerImpl::set_initial_state()
{
	if (!gl_resource_)
	{
		return;
	}

	set_mag_filter();
	set_min_filter();
	set_address_mode_u();
	set_address_mode_v();
	set_anisotropy();
}

//
// GlSamplerImpl
// =========================================================================


// =========================================================================
// Ren3dGlSamplerFactory
//

Ren3dGlSamplerUPtr Ren3dGlSamplerFactory::create(
	Ren3dGlContextPtr gl_context,
	const Ren3dSamplerCreateParam& param)
{
	return std::make_unique<GlSamplerImpl>(gl_context, param);
}

//
// Ren3dGlSamplerFactory
// =========================================================================


} // detail
} // bstone
