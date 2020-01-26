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
// OpenGL buffer object (implementation).
//
// !!! Internal usage only !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_buffer.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_buffer_manager.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_vao_manager.h"


namespace bstone
{
namespace detail
{


// =========================================================================
// GlBuffer
//

GlBuffer::GlBuffer() = default;

GlBuffer::~GlBuffer() = default;

//
// GlBuffer
// =========================================================================


// =========================================================================
// GlBufferImpl
//

struct GlBufferImplException :
	public Exception
{
	explicit GlBufferImplException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF] "} + message}
	{
	}
}; // GlBufferImplException


struct GlBufferImplCreateException :
	public Exception
{
	explicit GlBufferImplCreateException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_INIT] "} + message}
	{
	}
}; // GlBufferImplCreateException

struct GlBufferImplUpdateException :
	public Exception
{
	explicit GlBufferImplUpdateException(
		const char* const message)
		:
		Exception{std::string{"[GL_BUF_UPD] "} + message}
	{
	}
}; // GlBufferImplUpdateException


class GlBufferImpl final :
	public GlBuffer
{
public:
	GlBufferImpl(
		const GlBufferManagerPtr gl_buffer_manager,
		const Renderer3dBufferCreateParam& param);

	~GlBufferImpl() override;


	Renderer3dBufferKind get_kind() const noexcept override;

	Renderer3dBufferUsageKind get_usage_kind() const noexcept override;

	int get_size() const noexcept override;

	void update(
		const Renderer3dBufferUpdateParam& param) override;

	void set(
		const bool is_set) override;


private:
	static void resource_deleter(
		const GLuint& gl_name) noexcept;

	using BufferResource = UniqueResource<GLuint, resource_deleter>;

	const GlBufferManagerPtr gl_buffer_manager_;

	Renderer3dBufferKind kind_;
	Renderer3dBufferUsageKind usage_kind_;
	int size_;
	BufferResource gl_resource_;
	GLenum gl_target_;


	void validate_param(
		const Renderer3dBufferCreateParam& param);

	void validate_param(
		const Renderer3dBufferUpdateParam& param);

	static GLenum gl_get_target(
		const Renderer3dBufferKind kind);

	static GLenum gl_get_usage(
		const Renderer3dBufferUsageKind usage_kind);
}; // GlBufferImpl


using GlBufferImplUPtr = std::unique_ptr<GlBufferImpl>;

//
// GlBufferImpl
// =========================================================================


// =========================================================================
// GlBufferImpl
//

GlBufferImpl::GlBufferImpl(
	const GlBufferManagerPtr gl_buffer_manager,
	const Renderer3dBufferCreateParam& param)
	:
	gl_buffer_manager_{gl_buffer_manager},
	kind_{},
	usage_kind_{},
	size_{},
	gl_resource_{},
	gl_target_{}
{
	if (!gl_buffer_manager_)
	{
		throw GlBufferImplCreateException{"Null buffer manager."};
	}

	validate_param(param);

	const auto gl_context = gl_buffer_manager_->get_context();
	const auto& gl_device_features = gl_context->get_gl_device_features();

	auto gl_name = GLuint{};

	if (gl_device_features.dsa_is_available_)
	{
		glCreateBuffers(1, &gl_name);
		assert(!detail::GlRenderer3dUtils::was_errors());
	}
	else
	{
		glGenBuffers(1, &gl_name);
		assert(!detail::GlRenderer3dUtils::was_errors());
	}

	if (gl_name == 0)
	{
		throw GlBufferImplCreateException{"Failed to create a buffer object."};
	}

	kind_ = param.kind_;
	usage_kind_ = param.usage_kind_;
	size_ = param.size_;
	gl_resource_.reset(gl_name);
	gl_target_ = gl_get_target(param.kind_);

	const auto gl_usage = gl_get_usage(param.usage_kind_);

	if (gl_device_features.dsa_is_available_ &&
		gl_device_features.buffer_storage_is_available_)
	{
		glNamedBufferStorage(gl_resource_.get(), param.size_, nullptr, GL_DYNAMIC_STORAGE_BIT);
		assert(!detail::GlRenderer3dUtils::was_errors());
	}
	else
	{
		const auto gl_context = gl_buffer_manager_->get_context();
		const auto gl_vao_manager = gl_context->vao_get_manager();

		const auto is_index = (kind_ == Renderer3dBufferKind::index);

		if (is_index)
		{
			gl_vao_manager->push_current_set_default();
		}

		set(true);

		glBufferData(gl_target_, param.size_, nullptr, gl_usage);
		assert(!detail::GlRenderer3dUtils::was_errors());

		if (is_index)
		{
			gl_vao_manager->pop();
		}
	}
}

GlBufferImpl::~GlBufferImpl()
{
	gl_buffer_manager_->buffer_notify_destroy(this);
}

Renderer3dBufferKind GlBufferImpl::get_kind() const noexcept
{
	return kind_;
}

Renderer3dBufferUsageKind GlBufferImpl::get_usage_kind() const noexcept
{
	return usage_kind_;
}

int GlBufferImpl::get_size() const noexcept
{
	return size_;
}

void GlBufferImpl::set(
	const bool is_set)
{
	const auto gl_buffer = (is_set ? this : nullptr);
	const auto gl_resource = (is_set ? gl_resource_.get() : 0);

	if (gl_buffer_manager_->buffer_set_current(kind_, gl_buffer))
	{
		glBindBuffer(gl_target_, gl_resource);
		assert(!detail::GlRenderer3dUtils::was_errors());
	}
}

void GlBufferImpl::update(
	const Renderer3dBufferUpdateParam& param)
{
	validate_param(param);

	if (param.size_ == 0)
	{
		return;
	}

	const auto gl_context = gl_buffer_manager_->get_context();
	const auto& gl_device_features = gl_context->get_gl_device_features();

	if (gl_device_features.dsa_is_available_)
	{
		glNamedBufferSubData(
			gl_resource_.get(),
			param.offset_,
			param.size_,
			param.data_
		);

		assert(!detail::GlRenderer3dUtils::was_errors());
	}
	else
	{
		const auto gl_context = gl_buffer_manager_->get_context();
		const auto gl_vao_manager = gl_context->vao_get_manager();

		const auto is_index = (kind_ == Renderer3dBufferKind::index);

		if (is_index)
		{
			gl_vao_manager->push_current_set_default();
		}

		set(true);

		glBufferSubData(
			gl_target_,
			param.offset_,
			param.size_,
			param.data_
		);

		assert(!detail::GlRenderer3dUtils::was_errors());

		if (is_index)
		{
			gl_vao_manager->pop();
		}
	}
}

void GlBufferImpl::resource_deleter(
	const GLuint& gl_name) noexcept
{
	glDeleteBuffers(1, &gl_name);
	assert(!detail::GlRenderer3dUtils::was_errors());
}

void GlBufferImpl::validate_param(
	const Renderer3dBufferCreateParam& param)
{
	switch (param.kind_)
	{
		case Renderer3dBufferKind::index:
		case Renderer3dBufferKind::vertex:
			break;

		default:
			throw GlBufferImplCreateException{"Unsupported kind."};
	}

	switch (param.usage_kind_)
	{
		case Renderer3dBufferUsageKind::draw_streaming:
		case Renderer3dBufferUsageKind::draw_static:
		case Renderer3dBufferUsageKind::draw_dynamic:
			break;

		default:
			throw GlBufferImplCreateException{"Unsupported usage kind."};
	}

	if (param.size_ <= 0)
	{
		throw GlBufferImplCreateException{"Size out of range."};
	}
}

void GlBufferImpl::validate_param(
	const Renderer3dBufferUpdateParam& param)
{
	if (param.offset_ < 0)
	{
		throw GlBufferImplUpdateException{"Offset out of range."};
	}

	if (param.size_ < 0)
	{
		throw GlBufferImplUpdateException{"Size out of range."};
	}

	if (param.offset_ > size_)
	{
		throw GlBufferImplUpdateException{"Offset out of range."};
	}

	if (param.size_ > size_)
	{
		throw GlBufferImplUpdateException{"Size out of range."};
	}

	if ((param.offset_ + param.size_) > size_)
	{
		throw GlBufferImplUpdateException{"End offset out of range."};
	}

	if (param.size_ > 0 && !param.data_)
	{
		throw GlBufferImplUpdateException{"Null data."};
	}
}

GLenum GlBufferImpl::gl_get_target(
	const Renderer3dBufferKind kind)
{
	switch (kind)
	{
		case Renderer3dBufferKind::index:
			return GL_ELEMENT_ARRAY_BUFFER;

		case Renderer3dBufferKind::vertex:
			return GL_ARRAY_BUFFER;

		default:
			throw GlBufferImplException{"Unsupported kind."};
	}
}

GLenum GlBufferImpl::gl_get_usage(
	const Renderer3dBufferUsageKind usage_kind)
{
	switch (usage_kind)
	{
		case Renderer3dBufferUsageKind::draw_streaming:
			return GL_STREAM_DRAW;

		case Renderer3dBufferUsageKind::draw_static:
			return GL_STATIC_DRAW;

		case Renderer3dBufferUsageKind::draw_dynamic:
			return GL_DYNAMIC_DRAW;

		default:
			throw GlBufferImplException{"Unsupported usage kind."};
	}
}

//
// GlBufferImpl
// =========================================================================


// =========================================================================
// GlBufferFactory
//

GlBufferUPtr GlBufferFactory::create(
	const GlBufferManagerPtr gl_buffer_manager,
	const Renderer3dBufferCreateParam& param)
{
	return std::make_unique<GlBufferImpl>(gl_buffer_manager, param);
}

//
// GlBufferFactory
// =========================================================================


} // detail
} // bstone
