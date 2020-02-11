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
// OpenGL vertex array object manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_vao_mgr.h"

#include <stack>

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"
#include "bstone_uptr_resource_list.h"

#include "bstone_detail_r3d_gl_api.h"
#include "bstone_detail_r3d_gl_device_features.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_vao.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlVaoMgrImpl
//

class R3dGlVaoMgrImpl final :
	public R3dGlVaoMgr
{
public:
	R3dGlVaoMgrImpl(
		const R3dGlContextPtr gl_context,
		const R3dDeviceFeatures& device_features,
		const R3dGlDeviceFeatures& gl_device_features);

	~R3dGlVaoMgrImpl() override;


	R3dGlContextPtr get_context() const noexcept override;


	R3dGlVaoPtr create() override;

	void destroy(
		const R3dGlVaoPtr vao) override;

	void push_current_set_default() override;

	void pop() override;

	void bind(
		const R3dGlVaoPtr vao) override;


	bool set_current_index_buffer(
		const R3dBufferPtr index_buffer) override;


	void enable_location(
		const int location,
		const bool is_enable) override;


private:
	const R3dGlContextPtr gl_context_;
	const R3dDeviceFeatures& device_features_;
	const R3dGlDeviceFeatures& gl_device_features_;


	R3dGlVaoPtr vao_current_;
	R3dGlVaoUPtr vao_default_;

	using VaoStack = std::stack<R3dGlVaoPtr>;
	VaoStack vao_stack_;

	using Vaos = UPtrResourceList<R3dGlVao, R3dGlVaoFactory, Exception>;
	Vaos vaos_;


	void initialize_default_vao();

	void bind();

	void initialize();
}; // R3dGlVaoMgrImpl

using R3dGlVaoMgrImplPtr = R3dGlVaoMgrImpl*;
using R3dGlVaoMgrImplUPtr = std::unique_ptr<R3dGlVaoMgrImpl>;

//
// R3dGlVaoMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlVaoMgrImpl
//

R3dGlVaoMgrImpl::R3dGlVaoMgrImpl(
	const R3dGlContextPtr gl_context,
	const R3dDeviceFeatures& device_features,
	const R3dGlDeviceFeatures& gl_device_features)
	:
	gl_context_{gl_context},
	device_features_{device_features},
	gl_device_features_{gl_device_features},
	vao_current_{},
	vao_default_{},
	vao_stack_{},
	vaos_{}
{
	initialize();
}

R3dGlVaoMgrImpl::~R3dGlVaoMgrImpl() = default;

R3dGlContextPtr R3dGlVaoMgrImpl::get_context() const noexcept
{
	return gl_context_;
}

R3dGlVaoPtr R3dGlVaoMgrImpl::create()
{
	if (!gl_device_features_.vao_is_available_)
	{
		return vao_default_.get();
	}

	return vaos_.add(this);
}

void R3dGlVaoMgrImpl::destroy(
	const R3dGlVaoPtr vao)
{
	if (!gl_device_features_.vao_is_available_)
	{
		if (vao != vao_default_.get())
		{
			throw Exception{"Expected default VAO."};
		}

		return;
	}

	if (!vao)
	{
		throw Exception{"Null vertex array."};
	}

	if (vao_current_ == vao)
	{
		vao_current_ = vao_default_.get();
		bind();
	}

	vaos_.remove(vao);
}

void R3dGlVaoMgrImpl::push_current_set_default()
{
	vao_stack_.emplace(vao_current_);

	bind(vao_default_.get());
}

void R3dGlVaoMgrImpl::pop()
{
	if (vao_stack_.empty())
	{
		throw Exception{"Empty stack."};
	}

	const auto vao = vao_stack_.top();
	vao_stack_.pop();

	bind(vao);
}

void R3dGlVaoMgrImpl::bind(
	const R3dGlVaoPtr vao)
{
	if (!vao)
	{
		throw Exception{"Null VAO."};
	}

	if (vao_current_ == vao)
	{
		return;
	}

	vao_current_ = vao;
	bind();
}

bool R3dGlVaoMgrImpl::set_current_index_buffer(
	const R3dBufferPtr index_buffer)
{
	return vao_current_->set_current_index_buffer(index_buffer);
}

void R3dGlVaoMgrImpl::enable_location(
	const int location,
	const bool is_enable)
{
	vao_current_->enable_location(location, is_enable);
}

void R3dGlVaoMgrImpl::initialize_default_vao()
{
	vao_default_ = R3dGlVaoFactory::create(this);

	vao_current_ = vao_default_.get();
	bind();
}

void R3dGlVaoMgrImpl::bind()
{
	vao_current_->bind();
}

void R3dGlVaoMgrImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	initialize_default_vao();
}

//
// R3dGlVaoMgrImpl
// ==========================================================================


// ==========================================================================
// GlVaoMgrFactory
//

GlVaoMgrUPtr GlVaoMgrFactory::create(
	const R3dGlContextPtr gl_context,
	const R3dDeviceFeatures& device_features,
	const R3dGlDeviceFeatures& gl_device_features)
{
	return std::make_unique<R3dGlVaoMgrImpl>(
		gl_context,
		device_features,
		gl_device_features
	);
}

//
// GlVaoMgrFactory
// ==========================================================================


} // detail
} // bstone
