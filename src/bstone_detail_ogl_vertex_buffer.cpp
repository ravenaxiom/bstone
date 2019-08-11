/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

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
// OpenGL vertex buffer object (implementation).
//
// !!! Internal usage only !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_ogl_vertex_buffer.h"
#include "bstone_detail_renderer_utils.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// OglVertexBuffer
//

OglVertexBuffer::OglVertexBuffer(
	const OglStatePtr ogl_state)
	:
	ogl_state_{ogl_state}
{
	assert(ogl_state_ != nullptr);
}

RendererBufferUsageKind OglVertexBuffer::get_usage_kind() const
{
	return ogl_buffer_->get_usage_kind();
}

void OglVertexBuffer::update(
	const RendererVertexBufferUpdateParam& param)
{
	auto buffer_param = detail::OglBuffer::UpdateParam{};
	buffer_param.offset_ = param.offset_;
	buffer_param.size_ = param.size_;
	buffer_param.data_ = param.data_;

	ogl_buffer_->update(buffer_param);
}

const std::string& OglVertexBuffer::get_error_message() const
{
	return error_message_;
}

bool OglVertexBuffer::initialize(
	const RendererVertexBufferCreateParam& param)
{
	auto renderer_utils = detail::RendererUtils{};

	if (!renderer_utils.validate_vertex_buffer_create_param(param))
	{
		error_message_ = renderer_utils.get_error_message();

		return false;
	}

	auto ogl_buffer = detail::OglBufferUPtr{new detail::OglBuffer{}};

	auto buffer_param = detail::OglBuffer::InitializeParam{};
	buffer_param.kind_ = RendererBufferKind::vertex;
	buffer_param.usage_kind_ = param.usage_kind_;
	buffer_param.size_ = param.size_;
	buffer_param.ogl_state_ = ogl_state_;

	if (!ogl_buffer->initialize(buffer_param))
	{
		error_message_ = ogl_buffer->get_error_message();

		return false;
	}

	ogl_buffer_ = std::move(ogl_buffer);

	return true;
}

int OglVertexBuffer::get_size() const
{
	return ogl_buffer_->get_size();
}

void OglVertexBuffer::bind(
	const bool is_binded)
{
	if (is_binded)
	{
		ogl_buffer_->bind();
	}
	else
	{
		ogl_buffer_->unbind_target();
	}
}

//
// OglVertexBuffer
// ==========================================================================


} // detail
} // bstone