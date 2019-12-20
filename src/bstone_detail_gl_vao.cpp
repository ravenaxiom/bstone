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
// OpenGL vertex array object (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_vao.h"

#include "bstone_exception.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_gl_api.h"
#include "bstone_detail_gl_buffer.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_vao_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlVao
//

GlVao::GlVao() = default;

GlVao::~GlVao() = default;

//
// GlVao
// ==========================================================================


// ==========================================================================
// GlVaoImpl
//

class GlVaoImpl final :
	public GlVao
{
public:
	GlVaoImpl(
		const GlVaoManagerPtr manager);

	~GlVaoImpl() override;


	void bind() override;


	bool set_current_index_buffer(
		const Renderer3dBufferPtr index_buffer) override;


	void enable_location(
		const int location,
		const bool is_enable) override;


private:
	const GlVaoManagerPtr manager_;


	static void gl_deleter(
		const GLuint& gl_name) noexcept;

	using VaoResource = UniqueResource<GLuint, gl_deleter>;
	VaoResource gl_vao_resource_;

	Renderer3dBufferPtr index_buffer_;


	using EnabledLocations = std::vector<bool>;
	EnabledLocations enabled_locations_;


	void enable_location(
		const int location);

	void initialize_resource();

	void initialize_locations();

	void initialize();
}; // GlVaoImpl

using GlVaoImplPtr = GlVaoImpl*;
using GlVaoImplUPtr = std::unique_ptr<GlVaoImpl>;

//
// GlVaoImpl
// ==========================================================================


// ==========================================================================
// GlVaoImpl
//

GlVaoImpl::GlVaoImpl(
	const GlVaoManagerPtr manager)
	:
	manager_{manager},
	gl_vao_resource_{},
	index_buffer_{},
	enabled_locations_{}
{
	initialize();
}

GlVaoImpl::~GlVaoImpl() = default;

void GlVaoImpl::bind()
{
	if (!gl_vao_resource_)
	{
		return;
	}

	glBindVertexArray(gl_vao_resource_);
	assert(!GlRenderer3dUtils::was_errors());
}

bool GlVaoImpl::set_current_index_buffer(
	const Renderer3dBufferPtr index_buffer)
{
	if (index_buffer_ == index_buffer)
	{
		return false;
	}

	index_buffer_ = index_buffer;

	return true;
}

void GlVaoImpl::enable_location(
	const int location,
	const bool is_enable)
{
	if (enabled_locations_[location] == is_enable)
	{
		return;
	}

	enabled_locations_[location] = is_enable;
	enable_location(location);
}

void GlVaoImpl::gl_deleter(
	const GLuint& gl_name) noexcept
{
	glDeleteVertexArrays(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlVaoImpl::enable_location(
	const int location)
{
	const auto gl_function = (
		enabled_locations_[location] ?
		glEnableVertexAttribArray :
		glDisableVertexAttribArray
	);

	gl_function(location);
	assert(!GlRenderer3dUtils::was_errors());
}

void GlVaoImpl::initialize_resource()
{
	if (!manager_->get_gl_device_features().vao_is_available_)
	{
		return;
	}

	auto gl_name = GLuint{};

	glGenVertexArrays(1, &gl_name);
	assert(!GlRenderer3dUtils::was_errors());

	if (gl_name == 0)
	{
		throw Exception{"Failed to create OpenGL vertex array object."};
	}

	gl_vao_resource_.reset(gl_name);
}

void GlVaoImpl::initialize_locations()
{
	const auto& device_features = manager_->get_device_features();

	enabled_locations_.resize(device_features.vertex_input_max_locations_);
}

void GlVaoImpl::initialize()
{
	if (!manager_)
	{
		throw Exception{"Null manager."};
	}

	initialize_resource();
	initialize_locations();
}

//
// GlVaoImpl
// ==========================================================================


// ==========================================================================
// GlVaoDeleter
//

GlVaoDeleter::GlVaoDeleter(
	const GlVaoManagerPtr gl_vao_manager)
	:
	gl_vao_manager_{gl_vao_manager}
{
}

void GlVaoDeleter::operator()(
	const GlVaoPtr resource)
{
	gl_vao_manager_->destroy(resource);
}

//
// GlVaoDeleter
// ==========================================================================


// ==========================================================================
// GlVaoFactory
//

GlVaoUPtr GlVaoFactory::create(
	const GlVaoManagerPtr gl_vao_manager)
{
	return std::make_unique<GlVaoImpl>(gl_vao_manager);
}

//
// GlVaoFactory
// ==========================================================================


} // detail
} // bstone
