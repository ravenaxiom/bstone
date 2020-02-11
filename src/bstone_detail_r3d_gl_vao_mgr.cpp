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
// Ren3dGlVaoMgrImpl
//

class Ren3dGlVaoMgrImpl final :
	public Ren3dGlVaoMgr
{
public:
	Ren3dGlVaoMgrImpl(
		const Ren3dGlContextPtr gl_context,
		const Ren3dDeviceFeatures& device_features,
		const Ren3dGlDeviceFeatures& gl_device_features);

	~Ren3dGlVaoMgrImpl() override;


	Ren3dGlContextPtr get_context() const noexcept override;


	Ren3dGlVaoPtr create() override;

	void destroy(
		const Ren3dGlVaoPtr vao) override;

	void push_current_set_default() override;

	void pop() override;

	void bind(
		const Ren3dGlVaoPtr vao) override;


	bool set_current_index_buffer(
		const Ren3dBufferPtr index_buffer) override;


	void enable_location(
		const int location,
		const bool is_enable) override;


private:
	const Ren3dGlContextPtr gl_context_;
	const Ren3dDeviceFeatures& device_features_;
	const Ren3dGlDeviceFeatures& gl_device_features_;


	Ren3dGlVaoPtr vao_current_;
	Ren3dGlVaoUPtr vao_default_;

	using VaoStack = std::stack<Ren3dGlVaoPtr>;
	VaoStack vao_stack_;

	using Vaos = UPtrResourceList<Ren3dGlVao, Ren3dGlVaoFactory, Exception>;
	Vaos vaos_;


	void initialize_default_vao();

	void bind();

	void initialize();
}; // Ren3dGlVaoMgrImpl

using Ren3dGlVaoMgrImplPtr = Ren3dGlVaoMgrImpl*;
using Ren3dGlVaoMgrImplUPtr = std::unique_ptr<Ren3dGlVaoMgrImpl>;

//
// Ren3dGlVaoMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoMgrImpl
//

Ren3dGlVaoMgrImpl::Ren3dGlVaoMgrImpl(
	const Ren3dGlContextPtr gl_context,
	const Ren3dDeviceFeatures& device_features,
	const Ren3dGlDeviceFeatures& gl_device_features)
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

Ren3dGlVaoMgrImpl::~Ren3dGlVaoMgrImpl() = default;

Ren3dGlContextPtr Ren3dGlVaoMgrImpl::get_context() const noexcept
{
	return gl_context_;
}

Ren3dGlVaoPtr Ren3dGlVaoMgrImpl::create()
{
	if (!gl_device_features_.vao_is_available_)
	{
		return vao_default_.get();
	}

	return vaos_.add(this);
}

void Ren3dGlVaoMgrImpl::destroy(
	const Ren3dGlVaoPtr vao)
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

void Ren3dGlVaoMgrImpl::push_current_set_default()
{
	vao_stack_.emplace(vao_current_);

	bind(vao_default_.get());
}

void Ren3dGlVaoMgrImpl::pop()
{
	if (vao_stack_.empty())
	{
		throw Exception{"Empty stack."};
	}

	const auto vao = vao_stack_.top();
	vao_stack_.pop();

	bind(vao);
}

void Ren3dGlVaoMgrImpl::bind(
	const Ren3dGlVaoPtr vao)
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

bool Ren3dGlVaoMgrImpl::set_current_index_buffer(
	const Ren3dBufferPtr index_buffer)
{
	return vao_current_->set_current_index_buffer(index_buffer);
}

void Ren3dGlVaoMgrImpl::enable_location(
	const int location,
	const bool is_enable)
{
	vao_current_->enable_location(location, is_enable);
}

void Ren3dGlVaoMgrImpl::initialize_default_vao()
{
	vao_default_ = Ren3dGlVaoFactory::create(this);

	vao_current_ = vao_default_.get();
	bind();
}

void Ren3dGlVaoMgrImpl::bind()
{
	vao_current_->bind();
}

void Ren3dGlVaoMgrImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	initialize_default_vao();
}

//
// Ren3dGlVaoMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoMgrFactory
//

Ren3dGlVaoMgrUPtr Ren3dGlVaoMgrFactory::create(
	const Ren3dGlContextPtr gl_context,
	const Ren3dDeviceFeatures& device_features,
	const Ren3dGlDeviceFeatures& gl_device_features)
{
	return std::make_unique<Ren3dGlVaoMgrImpl>(
		gl_context,
		device_features,
		gl_device_features
	);
}

//
// Ren3dGlVaoMgrFactory
// ==========================================================================


} // detail
} // bstone
