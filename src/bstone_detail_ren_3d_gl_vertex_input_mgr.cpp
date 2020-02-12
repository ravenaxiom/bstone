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
// OpenGL vertex input manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_ren_3d_gl_vertex_input_mgr.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_vertex_input.h"
#include "bstone_detail_ren_3d_gl_vao_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGlVertexInputMgrImpl
//

class Ren3dGlVertexInputMgrImpl final :
	public Ren3dGlVertexInputMgr
{
public:
	Ren3dGlVertexInputMgrImpl(
		const Ren3dGlContextPtr gl_context);

	~Ren3dGlVertexInputMgrImpl() override;


	Ren3dGlContextPtr get_gl_context() const noexcept override;


	Ren3dVertexInputUPtr create(
		const Ren3dCreateVertexInputParam& param) override;

	void notify_destroy(
		const Ren3dVertexInputPtr vertex_input) noexcept override;

	void set(
		const Ren3dVertexInputPtr vertex_input) override;

	Ren3dBufferPtr get_current_index_buffer() const noexcept override;


	void enable_location(
		const int location,
		const bool is_enabled) override;

	void location_assign_begin() override;

	void location_assign_end() override;


private:
	const Ren3dGlContextPtr gl_context_;


	Ren3dGlVertexInputPtr vertex_input_current_;

	bool vertex_input_location_is_assigning_;
	using VertexInputAssignedLocations = std::vector<bool>;
	VertexInputAssignedLocations vertex_input_assigned_locations_;


	void initialize_vertex_input_locations();

	void initialize();

	void set();
}; // Ren3dGlVertexInputMgrImpl

using Ren3dGlVaoMgrImplPtr = Ren3dGlVertexInputMgrImpl*;
using Ren3dGlVaoMgrImplUPtr = std::unique_ptr<Ren3dGlVertexInputMgrImpl>;

//
// Ren3dGlVertexInputMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlVertexInputMgrImpl
//

Ren3dGlVertexInputMgrImpl::Ren3dGlVertexInputMgrImpl(
	const Ren3dGlContextPtr gl_context)
	:
	gl_context_{gl_context},
	vertex_input_current_{},
	vertex_input_location_is_assigning_{},
	vertex_input_assigned_locations_{}
{
	initialize();
}

Ren3dGlVertexInputMgrImpl::~Ren3dGlVertexInputMgrImpl() = default;

Ren3dGlContextPtr Ren3dGlVertexInputMgrImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

Ren3dVertexInputUPtr Ren3dGlVertexInputMgrImpl::create(
	const Ren3dCreateVertexInputParam& param)
{
	return Ren3dGlVertexInputFactory::create(this, param);
}

void Ren3dGlVertexInputMgrImpl::notify_destroy(
	const Ren3dVertexInputPtr vertex_input) noexcept
{
	if (vertex_input_current_ == vertex_input)
	{
		vertex_input_current_ = nullptr;
	}
}

void Ren3dGlVertexInputMgrImpl::set(
	const Ren3dVertexInputPtr vertex_input)
{
	if (vertex_input_current_ == vertex_input)
	{
		return;
	}

	vertex_input_current_ = static_cast<Ren3dGlVertexInputPtr>(vertex_input);
	set();
}

Ren3dBufferPtr Ren3dGlVertexInputMgrImpl::get_current_index_buffer() const noexcept
{
	if (!vertex_input_current_)
	{
		return nullptr;
	}

	return vertex_input_current_->get_index_buffer();
}

void Ren3dGlVertexInputMgrImpl::enable_location(
	const int location,
	const bool is_enabled)
{
	if (vertex_input_location_is_assigning_)
	{
		vertex_input_assigned_locations_[location] = true;

		return;
	}

	const auto vao_manager = gl_context_->vao_get_manager();

	vao_manager->enable_location(location, is_enabled);
}

void Ren3dGlVertexInputMgrImpl::location_assign_begin()
{
	if (vertex_input_location_is_assigning_)
	{
		throw Exception{"Already assigning."};
	}

	vertex_input_location_is_assigning_ = true;

	std::fill(
		vertex_input_assigned_locations_.begin(),
		vertex_input_assigned_locations_.end(),
		false
	);
}

void Ren3dGlVertexInputMgrImpl::location_assign_end()
{
	if (!vertex_input_location_is_assigning_)
	{
		throw Exception{"Not assigning."};
	}

	vertex_input_location_is_assigning_ = false;

	const auto& device_features = gl_context_->get_device_features();

	for (int i = 0; i < device_features.max_vertex_input_locations_; ++i)
	{
		enable_location(i, vertex_input_assigned_locations_[i]);
	}
}

void Ren3dGlVertexInputMgrImpl::initialize_vertex_input_locations()
{
	const auto& device_features = gl_context_->get_device_features();

	vertex_input_assigned_locations_.resize(device_features.max_vertex_input_locations_);

	for (int i = 0; i < device_features.max_vertex_input_locations_; ++i)
	{
		enable_location(i, false);
	}
}

void Ren3dGlVertexInputMgrImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	initialize_vertex_input_locations();
}

void Ren3dGlVertexInputMgrImpl::set()
{
	if (!vertex_input_current_)
	{
		return;
	}

	vertex_input_current_->bind();
}

//
// Ren3dGlVertexInputMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoMgrFactory
//

Ren3dGlVertexInputMgrUPtr Ren3dGlVertexInputMgrFactory::create(
	const Ren3dGlContextPtr gl_context)
{
	return std::make_unique<Ren3dGlVertexInputMgrImpl>(gl_context);
}

//
// Ren3dGlVaoMgrFactory
// ==========================================================================


} // detail
} // bstone
