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
#include "bstone_detail_gl_vao_manager.h"

#include <stack>

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"
#include "bstone_uptr_resource_list.h"

#include "bstone_detail_gl_api.h"
#include "bstone_detail_gl_device_features.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_vao.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlVaoManagerImpl
//

class GlVaoManagerImpl final :
	public GlVaoManager
{
public:
	GlVaoManagerImpl(
		const GlContextPtr gl_context,
		const Renderer3dDeviceFeatures& device_features,
		const GlDeviceFeatures& gl_device_features);

	~GlVaoManagerImpl() override;


	GlContextPtr get_context() const noexcept override;


	GlVaoPtr create() override;

	void destroy(
		const GlVaoPtr vao) override;

	void push_current_set_default() override;

	void pop() override;

	void bind(
		const GlVaoPtr vao) override;


	bool set_current_index_buffer(
		const Renderer3dBufferPtr index_buffer) override;


	void enable_location(
		const int location,
		const bool is_enable) override;


private:
	const GlContextPtr gl_context_;
	const Renderer3dDeviceFeatures& device_features_;
	const GlDeviceFeatures& gl_device_features_;


	GlVaoPtr vao_current_;
	GlVaoUPtr vao_default_;

	using VaoStack = std::stack<GlVaoPtr>;
	VaoStack vao_stack_;

	using Vaos = UPtrResourceList<GlVao, GlVaoFactory, Exception>;
	Vaos vaos_;


	void initialize_default_vao();

	void bind();

	void initialize();
}; // GlVaoManagerImpl

using GlVaoManagerImplPtr = GlVaoManagerImpl*;
using GlVaoManagerImplUPtr = std::unique_ptr<GlVaoManagerImpl>;

//
// GlVaoManagerImpl
// ==========================================================================


// ==========================================================================
// GlVaoManagerImpl
//

GlVaoManagerImpl::GlVaoManagerImpl(
	const GlContextPtr gl_context,
	const Renderer3dDeviceFeatures& device_features,
	const GlDeviceFeatures& gl_device_features)
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

GlVaoManagerImpl::~GlVaoManagerImpl() = default;

GlContextPtr GlVaoManagerImpl::get_context() const noexcept
{
	return gl_context_;
}

GlVaoPtr GlVaoManagerImpl::create()
{
	if (!gl_device_features_.vao_is_available_)
	{
		return vao_default_.get();
	}

	return vaos_.add(this);
}

void GlVaoManagerImpl::destroy(
	const GlVaoPtr vao)
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

void GlVaoManagerImpl::push_current_set_default()
{
	vao_stack_.emplace(vao_current_);

	bind(vao_default_.get());
}

void GlVaoManagerImpl::pop()
{
	if (vao_stack_.empty())
	{
		throw Exception{"Empty stack."};
	}

	const auto vao = vao_stack_.top();
	vao_stack_.pop();

	bind(vao);
}

void GlVaoManagerImpl::bind(
	const GlVaoPtr vao)
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

bool GlVaoManagerImpl::set_current_index_buffer(
	const Renderer3dBufferPtr index_buffer)
{
	return vao_current_->set_current_index_buffer(index_buffer);
}

void GlVaoManagerImpl::enable_location(
	const int location,
	const bool is_enable)
{
	vao_current_->enable_location(location, is_enable);
}

void GlVaoManagerImpl::initialize_default_vao()
{
	vao_default_ = GlVaoFactory::create(this);

	vao_current_ = vao_default_.get();
	bind();
}

void GlVaoManagerImpl::bind()
{
	vao_current_->bind();
}

void GlVaoManagerImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL state."};
	}

	initialize_default_vao();
}

//
// GlVaoManagerImpl
// ==========================================================================


// ==========================================================================
// GlVaoManagerFactory
//

GlVaoManagerUPtr GlVaoManagerFactory::create(
	const GlContextPtr gl_context,
	const Renderer3dDeviceFeatures& device_features,
	const GlDeviceFeatures& gl_device_features)
{
	return std::make_unique<GlVaoManagerImpl>(
		gl_context,
		device_features,
		gl_device_features
	);
}

//
// GlVaoManagerFactory
// ==========================================================================


} // detail
} // bstone
