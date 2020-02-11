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
// OpenGL 2D texture (implementation).
//
// !!! Internal usage only !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_texture_2d.h"

#include "bstone_exception.h"
#include "bstone_r3d_limits.h"
#include "bstone_r3d_tests.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_r3d_gl_api.h"
#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_sampler_mgr.h"
#include "bstone_detail_r3d_gl_texture_mgr.h"
#include "bstone_detail_r3d_utils.h"


namespace bstone
{
namespace detail
{


// =========================================================================
// GlTexture2dImpl
//

class GlTexture2dImpl final :
	public R3dGlTexture2d
{
public:
	GlTexture2dImpl(
		const R3dGlTextureMgrPtr gl_texture_manager,
		const R3dTexture2dCreateParam& param);

	~GlTexture2dImpl() override;


	void update(
		const R3dTexture2dUpdateParam& param) override;

	void generate_mipmaps() override;


	void set() override;

	void update_sampler_state(
		const R3dSamplerState& new_sampler_state) override;


private:
	const R3dGlTextureMgrPtr gl_texture_manager_;
	const R3dDeviceFeatures& device_features_;
	const R3dGlDeviceFeatures& gl_device_features_;

	R3dPixelFormat pixel_format_;
	GLenum gl_internal_format_;
	GLenum gl_format_;
	GLenum gl_type_;

	int width_;
	int height_;

	int mipmap_count_;

	R3dSamplerState sampler_state_;


	void validate(
		const R3dTexture2dCreateParam& param);

	void validate(
		const R3dTexture2dUpdateParam& param);


	void bind() override;

	void upload_mipmap(
		const int mipmap_level,
		const int width,
		const int height,
		const void* const src_data);

	void set_mag_filter();

	void set_min_filter();

	void set_address_mode(
		const R3dTextureAxis texture_axis,
		const R3dAddressMode address_mode);

	void set_address_mode_u();

	void set_address_mode_v();

	void set_anisotropy();

	void set_sampler_state_defaults();


	static void texture_resource_deleter(
		const GLuint& gl_name) noexcept;

	using GlTextureResource = UniqueResource<GLuint, texture_resource_deleter>;
	GlTextureResource gl_resource_;
}; // R3dGlTexture2d

using GlTexture2dImplPtr = GlTexture2dImpl*;
using GlTexture2dImplUPtr = std::unique_ptr<GlTexture2dImpl>;

//
// GlTexture2dImpl
// =========================================================================


// ==========================================================================
// GlTexture2dImpl
//

GlTexture2dImpl::GlTexture2dImpl(
	const R3dGlTextureMgrPtr gl_texture_manager,
	const R3dTexture2dCreateParam& param)
	:
	gl_texture_manager_{gl_texture_manager},
	device_features_{gl_texture_manager_->get_gl_context()->get_device_features()},
	gl_device_features_{gl_texture_manager_->get_gl_context()->get_gl_device_features()},
	pixel_format_{},
	gl_internal_format_{},
	gl_format_{},
	gl_type_{},
	width_{},
	height_{},
	mipmap_count_{},
	sampler_state_{},
	gl_resource_{}
{
	validate(param);

	pixel_format_ = param.pixel_format_;

	const auto is_es = (gl_device_features_.context_kind_ == R3dGlContextKind::es);

	switch (pixel_format_)
	{
		case R3dPixelFormat::rgba_8_unorm:
			gl_internal_format_ = (is_es ? GL_RGBA : GL_RGBA8);
			gl_format_ = GL_RGBA;
			gl_type_ = GL_UNSIGNED_BYTE;
			break;

		default:
			throw Exception{"Unsupported image format."};
	}

	width_ = param.width_;
	height_ = param.height_;
	mipmap_count_ = param.mipmap_count_;

	const auto max_mipmap_count = R3dUtils::calculate_mipmap_count(width_, height_);

	if (mipmap_count_ > max_mipmap_count)
	{
		throw Exception{"Mipmap count out of range."};
	}

	auto gl_name = GLuint{};

	if (gl_device_features_.dsa_is_available_)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &gl_name);
		R3dGlError::ensure_debug();
	}
	else
	{
		glGenTextures(1, &gl_name);
		R3dGlError::ensure_debug();
	}

	gl_resource_.reset(gl_name);

	if (!gl_resource_)
	{
		throw Exception{"Failed to create OpenGL 2D-texture object."};
	}

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureParameteri(gl_resource_.get(), GL_TEXTURE_BASE_LEVEL, 0);
		R3dGlError::ensure_debug();

		glTextureParameteri(gl_resource_.get(), GL_TEXTURE_MAX_LEVEL, mipmap_count_ - 1);
		R3dGlError::ensure_debug();
	}
	else
	{
		bind();

		gl_texture_manager_->set_active(this);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		R3dGlError::ensure_debug();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap_count_ - 1);
		R3dGlError::ensure_debug();
	}

	set_sampler_state_defaults();

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureStorage2D(
			gl_resource_.get(),
			mipmap_count_,
			gl_internal_format_,
			width_,
			height_
		);

		R3dGlError::ensure_debug();
	}
	else
	{
		auto mipmap_width = width_;
		auto mipmap_height = height_;

		for (int i_mipmap = 0; i_mipmap < mipmap_count_; ++i_mipmap)
		{
			glTexImage2D(
				GL_TEXTURE_2D, // target
				i_mipmap, // level
				gl_internal_format_, // internal format
				mipmap_width, // width
				mipmap_height, // height
				0, // border
				gl_format_, // format
				gl_type_, // type
				nullptr // pixels
			);

			R3dGlError::ensure_debug();

			if (mipmap_width > 1)
			{
				mipmap_width /= 2;
			}

			if (mipmap_height > 1)
			{
				mipmap_height /= 2;
			}
		}
	}
}

GlTexture2dImpl::~GlTexture2dImpl()
{
	gl_texture_manager_->notify_destroy(this);
}

void GlTexture2dImpl::update(
	const R3dTexture2dUpdateParam& param)
{
	validate(param);

	if (param.mipmap_level_ >= mipmap_count_)
	{
		throw Exception{"Mipmap level out of range."};
	}

	if (!gl_device_features_.dsa_is_available_)
	{
		gl_texture_manager_->set(this);
	}

	auto mipmap_width = width_;
	auto mipmap_height = height_;

	for (auto i = 0; i < param.mipmap_level_; ++i)
	{
		if (mipmap_width > 1)
		{
			mipmap_width /= 2;
		}

		if (mipmap_height > 1)
		{
			mipmap_height /= 2;
		}
	}

	upload_mipmap(param.mipmap_level_, mipmap_width, mipmap_height, param.image_);
}

void GlTexture2dImpl::generate_mipmaps()
{
	if (mipmap_count_ <= 1)
	{
		throw Exception{"Base mipmap."};
	}

	if (!device_features_.mipmap_is_available_)
	{
		throw Exception{"Mipmap generation not available."};
	}

	if (gl_device_features_.dsa_is_available_)
	{
		glGenerateTextureMipmap(gl_resource_.get());
		R3dGlError::ensure_debug();
	}
	else
	{
		gl_texture_manager_->set(this);

		R3dGlUtils::mipmap_generate(
			GL_TEXTURE_2D,
			device_features_,
			gl_device_features_);
	}
}

void GlTexture2dImpl::texture_resource_deleter(
	const GLuint& gl_name) noexcept
{
	glDeleteTextures(1, &gl_name);
	R3dGlError::ensure_debug();
}

void GlTexture2dImpl::validate(
	const R3dTexture2dCreateParam& param)
{
	switch (param.pixel_format_)
	{
		case R3dPixelFormat::rgba_8_unorm:
			break;

		default:
			throw Exception{"Invalid pixel format."};
	}

	if (param.width_ <= 0)
	{
		throw Exception{"Invalid width."};
	}

	if (param.height_ <= 0)
	{
		throw Exception{"Invalid height."};
	}

	if (param.mipmap_count_ <= 0)
	{
		throw Exception{"Invalid mipmap count."};
	}
}

void GlTexture2dImpl::validate(
	const R3dTexture2dUpdateParam& param)
{
	if (param.mipmap_level_ < 0 ||
		param.mipmap_level_ >= R3dLimits::max_mipmap_count)
	{
		throw Exception{"Mipmap level out of range."};
	}

	if (!param.image_)
	{
		throw Exception{"Null image data."};
	}
}

void GlTexture2dImpl::bind()
{
	glBindTexture(GL_TEXTURE_2D, gl_resource_.get());
	R3dGlError::ensure_debug();
}

void GlTexture2dImpl::upload_mipmap(
	const int mipmap_level,
	const int width,
	const int height,
	const void* const src_data)
{
	if (gl_device_features_.dsa_is_available_)
	{
		glTextureSubImage2D(
			gl_resource_.get(), // target
			mipmap_level, // level
			0, // xoffset
			0, // yoffset
			width, // width
			height, // height
			gl_format_, // format
			gl_type_, // type
			src_data // pixels
		);

		R3dGlError::ensure_debug();
	}
	else
	{
		glTexSubImage2D(
			GL_TEXTURE_2D, // target
			mipmap_level, // level
			0, // xoffset
			0, // yoffset
			width, // width
			height, // height
			gl_format_, // format
			gl_type_, // type
			src_data // pixels
		);

		R3dGlError::ensure_debug();
	}
}

void GlTexture2dImpl::set()
{
	bind();

	gl_texture_manager_->set_active(this);

	if (!gl_device_features_.dsa_is_available_)
	{
		const auto sampler_manger = gl_texture_manager_->get_gl_context()->sampler_get_manager();
		const auto& sampler_state = sampler_manger->get_current_state();
		update_sampler_state(sampler_state);
	}
}

void GlTexture2dImpl::set_mag_filter()
{
	const auto gl_mag_filter = R3dGlUtils::filter_get_mag(sampler_state_.mag_filter_);

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureParameteri(gl_resource_.get(), GL_TEXTURE_MAG_FILTER, gl_mag_filter);
		R3dGlError::ensure_debug();
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
		R3dGlError::ensure_debug();
	}
}

void GlTexture2dImpl::set_min_filter()
{
	const auto gl_min_filter = R3dGlUtils::filter_get_min(
		sampler_state_.min_filter_,
		sampler_state_.mipmap_mode_
	);

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureParameteri(gl_resource_.get(), GL_TEXTURE_MIN_FILTER, gl_min_filter);
		R3dGlError::ensure_debug();
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_min_filter);
		R3dGlError::ensure_debug();
	}
}

void GlTexture2dImpl::set_address_mode(
	const R3dTextureAxis texture_axis,
	const R3dAddressMode address_mode)
{
	const auto gl_wrap_axis = R3dGlUtils::texture_wrap_get_axis(texture_axis);
	const auto gl_address_mode = R3dGlUtils::address_mode_get(address_mode);

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureParameteri(gl_resource_.get(), gl_wrap_axis, gl_address_mode);
		R3dGlError::ensure_debug();
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, gl_wrap_axis, gl_address_mode);
		R3dGlError::ensure_debug();
	}
}

void GlTexture2dImpl::set_address_mode_u()
{
	set_address_mode(R3dTextureAxis::u, sampler_state_.address_mode_u_);
}

void GlTexture2dImpl::set_address_mode_v()
{
	set_address_mode(R3dTextureAxis::v, sampler_state_.address_mode_v_);
}

void GlTexture2dImpl::set_anisotropy()
{
	if (!device_features_.anisotropy_is_available_)
	{
		return;
	}

	auto anisotropy = sampler_state_.anisotropy_;

	if (anisotropy < R3dLimits::anisotropy_min_off)
	{
		anisotropy = R3dLimits::anisotropy_min_off;
	}
	else if (anisotropy > device_features_.anisotropy_max_degree_)
	{
		anisotropy = device_features_.anisotropy_max_degree_;
	}

	const auto gl_anisotropy = static_cast<GLfloat>(anisotropy);

	if (gl_device_features_.dsa_is_available_)
	{
		glTextureParameterf(gl_resource_.get(), GL_TEXTURE_MAX_ANISOTROPY, gl_anisotropy);
		R3dGlError::ensure_debug();
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, gl_anisotropy);
		R3dGlError::ensure_debug();
	}
}

void GlTexture2dImpl::update_sampler_state(
	const R3dSamplerState& new_sampler_state)
{
	auto is_modified = false;

	// Magnification filter.
	//
	auto is_mag_filter_modified = false;

	if (sampler_state_.mag_filter_ != new_sampler_state.mag_filter_)
	{
		is_modified = true;
		is_mag_filter_modified = true;

		sampler_state_.mag_filter_ = new_sampler_state.mag_filter_;
	}

	// Minification filter.
	//
	auto is_min_filter_modified = false;

	if (sampler_state_.min_filter_ != new_sampler_state.min_filter_ ||
		sampler_state_.mipmap_mode_ != new_sampler_state.mipmap_mode_)
	{
		is_modified = true;
		is_min_filter_modified = true;

		sampler_state_.min_filter_ = new_sampler_state.min_filter_;
		sampler_state_.mipmap_mode_ = new_sampler_state.mipmap_mode_;
	}

	// U-axis address mode.
	//
	auto is_address_mode_u = false;

	if (sampler_state_.address_mode_u_ != new_sampler_state.address_mode_u_)
	{
		is_modified = true;
		is_address_mode_u = true;

		sampler_state_.address_mode_u_ = new_sampler_state.address_mode_u_;
	}

	// V-axis address mode.
	//
	auto is_address_mode_v = false;

	if (sampler_state_.address_mode_v_ != new_sampler_state.address_mode_v_)
	{
		is_modified = true;
		is_address_mode_v = true;

		sampler_state_.address_mode_v_ = new_sampler_state.address_mode_v_;
	}

	// Anisotropy.
	//
	auto is_anisotropy = false;

	if (sampler_state_.anisotropy_ != new_sampler_state.anisotropy_)
	{
		is_modified = true;
		is_anisotropy = true;

		sampler_state_.anisotropy_ = new_sampler_state.anisotropy_;
	}


	// Modify.
	//
	if (is_modified)
	{
		gl_texture_manager_->set(this);

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
}

void GlTexture2dImpl::set_sampler_state_defaults()
{
	sampler_state_.mag_filter_ = R3dFilterKind::nearest;
	set_mag_filter();

	sampler_state_.min_filter_ = R3dFilterKind::nearest;
	sampler_state_.mipmap_mode_ = R3dMipmapMode::none;
	set_min_filter();

	sampler_state_.address_mode_u_ = R3dAddressMode::clamp;
	set_address_mode_u();

	sampler_state_.address_mode_v_ = R3dAddressMode::clamp;
	set_address_mode_v();

	sampler_state_.anisotropy_ = R3dLimits::anisotropy_min_off;
	set_anisotropy();
}

//
// GlTexture2dImpl
// ==========================================================================


// =========================================================================
// R3dGlTexture2dFactory
//

R3dGlTexture2dUPtr R3dGlTexture2dFactory::create(
	const R3dGlTextureMgrPtr gl_texture_manager,
	const R3dTexture2dCreateParam& param)
{
	return std::make_unique<GlTexture2dImpl>(gl_texture_manager, param);
}

//
// R3dGlTexture2dFactory
// =========================================================================


} // detail
} // bstone
