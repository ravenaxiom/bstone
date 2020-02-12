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
// OpenGL vertex input (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_ren_3d_gl_vertex_input.h"

#include <algorithm>

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_buffer.h"
#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_device_features.h"
#include "bstone_detail_ren_3d_gl_error.h"
#include "bstone_detail_ren_3d_gl_utils.h"
#include "bstone_detail_ren_3d_gl_vao.h"
#include "bstone_detail_ren_3d_gl_vao_mgr.h"
#include "bstone_detail_ren_3d_gl_vertex_input_mgr.h"
#include "bstone_detail_ren_3d_utils.h"


namespace bstone
{
namespace detail
{


// =========================================================================
// GlVertexInputImpl
//

class GlVertexInputImpl final :
	public Ren3dGlVertexInput
{
public:
	GlVertexInputImpl(
		const Ren3dGlVertexInputMgrPtr vertex_input_manager,
		const Ren3dCreateVertexInputParam& param);

	~GlVertexInputImpl() override;


	Ren3dBufferPtr get_index_buffer() const noexcept override;

	void bind() override;


private:
	const Ren3dGlVertexInputMgrPtr vertex_input_manager_;
	const Ren3dGlVaoMgrPtr vao_manager_;
	const Ren3dDeviceFeatures& device_features_;
	const Ren3dGlDeviceFeatures& gl_device_features_;

	Ren3dGlBufferPtr index_buffer_;
	Ren3dVertexAttribDescriptions attribute_descriptions_;


	Ren3dGlVaoResource gl_resource_;


	void initialize_vao();

	void initialize(
		const Ren3dCreateVertexInputParam& param);

	void assign_default_attribute(
		const Ren3dVertexAttribDescription& attribute_description);

	void assign_regular_attribute(
		const Ren3dVertexAttribDescription& attribute_description);

	void assign_attribute(
		const Ren3dVertexAttribDescription& attribute_description);

	void bind_internal();
}; // GlVertexInputImpl

using GlVertexInputImplPtr = GlVertexInputImpl*;
using GlVertexInputImplUPtr = std::unique_ptr<GlVertexInputImpl>;

//
// GlVertexInputImpl
// =========================================================================


// =========================================================================
// GlVertexInputImpl
//

GlVertexInputImpl::GlVertexInputImpl(
	const Ren3dGlVertexInputMgrPtr vertex_input_manager,
	const Ren3dCreateVertexInputParam& param)
	:
	vertex_input_manager_{vertex_input_manager},
	vao_manager_{vertex_input_manager->get_gl_context()->vao_get_manager()},
	device_features_{vertex_input_manager->get_gl_context()->get_device_features()},
	gl_device_features_{vertex_input_manager->get_gl_context()->get_gl_device_features()},
	index_buffer_{},
	attribute_descriptions_{},
	gl_resource_{nullptr, Ren3dGlVaoDeleter{vertex_input_manager->get_gl_context()->vao_get_manager()}}
{
	initialize(param);
}

GlVertexInputImpl::~GlVertexInputImpl()
{
	vertex_input_manager_->notify_destroy(this);
}

Ren3dBufferPtr GlVertexInputImpl::get_index_buffer() const noexcept
{
	return index_buffer_;
}

void GlVertexInputImpl::bind()
{
	const auto gl_context = vertex_input_manager_->get_gl_context();

	if (gl_device_features_.vao_is_available_)
	{
		for (const auto& attribute_description : attribute_descriptions_)
		{
			if (attribute_description.is_default_)
			{
				assign_default_attribute(attribute_description);
			}
		}

		vao_manager_->bind(gl_resource_.get());
	}
	else
	{
		bind_internal();
	}
}

void GlVertexInputImpl::initialize_vao()
{
	gl_resource_.reset(vao_manager_->create());

	vao_manager_->bind(gl_resource_.get());

	index_buffer_->set(true);

	if (gl_device_features_.vao_is_available_)
	{
		for (const auto& attribute_description : attribute_descriptions_)
		{
			if (!attribute_description.is_default_)
			{
				assign_regular_attribute(attribute_description);
			}
		}
	}
}

void GlVertexInputImpl::initialize(
	const Ren3dCreateVertexInputParam& param)
{
	const auto max_locations = device_features_.vertex_input_max_locations_;

	Ren3dUtils::vertex_input_validate_param(max_locations, param);

	const auto is_location_out_of_range = std::any_of(
		param.attribute_descriptions_.cbegin(),
		param.attribute_descriptions_.cend(),
		[=](const auto& item)
		{
			return item.location_ < 0 || item.location_ >= max_locations;
		}
	);

	if (is_location_out_of_range)
	{
		throw Exception{"Location out of range."};
	}

	index_buffer_ = static_cast<Ren3dGlBufferPtr>(param.index_buffer_);
	attribute_descriptions_ = param.attribute_descriptions_;

	initialize_vao();
}

void GlVertexInputImpl::assign_default_attribute(
	const Ren3dVertexAttribDescription& attribute_description)
{
	glVertexAttrib4fv(
		attribute_description.location_,
		attribute_description.default_value_.data()
	);

	Ren3dGlError::ensure_debug();
}

void GlVertexInputImpl::assign_regular_attribute(
	const Ren3dVertexAttribDescription& attribute_description)
{
	auto gl_component_count = GLint{};
	auto gl_component_format = GLenum{};
	auto gl_is_normalized = GLenum{};

	switch (attribute_description.format_)
	{
		case Ren3dVertexAttribFormat::rgba_8_unorm:
			gl_is_normalized = true;
			gl_component_count = 4;
			gl_component_format = GL_UNSIGNED_BYTE;
			break;

		case Ren3dVertexAttribFormat::rg_32_sfloat:
			gl_component_count = 2;
			gl_component_format = GL_FLOAT;
			break;

		case Ren3dVertexAttribFormat::rgb_32_sfloat:
			gl_component_count = 3;
			gl_component_format = GL_FLOAT;
			break;

		default:
			throw Exception{"Invalid format."};
	}

	const auto gl_context = vertex_input_manager_->get_gl_context();

	const auto vertex_input_manager = gl_context->vertex_input_get_manager();

	vertex_input_manager->enable_location(attribute_description.location_, true);

	auto vertex_buffer = static_cast<Ren3dGlBufferPtr>(attribute_description.vertex_buffer_);

	vertex_buffer->set(true);

	const auto vertex_buffer_data = reinterpret_cast<const void*>(static_cast<std::intptr_t>(attribute_description.offset_));

	glVertexAttribPointer(
		attribute_description.location_,
		gl_component_count,
		gl_component_format,
		gl_is_normalized,
		attribute_description.stride_,
		vertex_buffer_data
	);

	Ren3dGlError::ensure_debug();
}

void GlVertexInputImpl::assign_attribute(
	const Ren3dVertexAttribDescription& attribute_description)
{
	if (attribute_description.is_default_)
	{
		assign_default_attribute(attribute_description);
	}
	else
	{
		assign_regular_attribute(attribute_description);
	}
}

void GlVertexInputImpl::bind_internal()
{
	const auto gl_context = vertex_input_manager_->get_gl_context();
	const auto vertex_input_manager = gl_context->vertex_input_get_manager();

	vertex_input_manager->location_assign_begin();

	for (const auto& attribute_description : attribute_descriptions_)
	{
		assign_attribute(attribute_description);
	}

	vertex_input_manager->location_assign_end();
}

//
// GlVertexInputImpl
// =========================================================================


// =========================================================================
// Ren3dGlVertexInputFactory
//

Ren3dGlVertexInputUPtr Ren3dGlVertexInputFactory::create(
	const Ren3dGlVertexInputMgrPtr vertex_input_manager,
	const Ren3dCreateVertexInputParam& param)
{
	return std::make_unique<GlVertexInputImpl>(vertex_input_manager, param);
}

//
// Ren3dGlVertexInputFactory
// =========================================================================


} // detail
} // bstone
