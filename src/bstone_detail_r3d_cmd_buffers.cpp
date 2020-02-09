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
// 3D renderer's command buffer list (implementaion).
//


#include "bstone_precompiled.h"

#include "bstone_detail_r3d_cmd_buffers.h"

#include <algorithm>
#include <vector>

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandBuffersImplException
//

class Renderer3dCommandBuffersImplException :
	public Exception
{
public:
	explicit Renderer3dCommandBuffersImplException(
		const char* const message)
		:
		Exception{std::string{"[R3D_CMD_BUFS] "} + message}
	{
	}
}; // Renderer3dCommandBuffersImplException

//
// Renderer3dCommandBuffersImplException
// ==========================================================================


// ==========================================================================
// Renderer3dCommandBuffersImpl
//

class Renderer3dCommandBuffersImpl final :
	public Renderer3dCommandBuffers
{
public:
	Renderer3dCommandBuffersImpl();

	~Renderer3dCommandBuffersImpl() override = default;


	int get_count() const noexcept override;

	bstone::Renderer3dCommandBufferPtr enqueue(
		const Renderer3dCommandQueueEnqueueParam& param) override;

	void dequeue(
		const bstone::Renderer3dCommandBufferPtr buffer) override;

	bstone::Renderer3dCommandBufferPtr get(
		const int index) override;


	Iterator begin() noexcept override;

	CIterator cbegin() const noexcept override;

	Iterator end() noexcept override;

	CIterator cend() const noexcept override;


private:
	static constexpr auto reserve_count = 16;


	using Buffers = std::vector<Renderer3dCommandBufferUPtr>;
	Buffers buffers_;
}; // Renderer3dCommandBuffersImpl

using Renderer3dCommandBuffersImplPtr = Renderer3dCommandBuffersImpl*;
using Renderer3dCommandBuffersImplUPtr = std::unique_ptr<Renderer3dCommandBuffersImpl>;

//
// Renderer3dCommandBuffersImpl
// ==========================================================================


// ==========================================================================
// Renderer3dCommandBuffersImpl
//

Renderer3dCommandBuffersImpl::Renderer3dCommandBuffersImpl()
	:
	buffers_{}
{
	buffers_.reserve(reserve_count);
}

int Renderer3dCommandBuffersImpl::get_count() const noexcept
{
	return static_cast<int>(buffers_.size());
}

bstone::Renderer3dCommandBufferPtr Renderer3dCommandBuffersImpl::enqueue(
	const Renderer3dCommandQueueEnqueueParam& param)
{
	auto buffer = std::make_unique<Renderer3dCommandBufferImpl>(param);

	buffers_.push_back(std::move(buffer));

	return buffers_.back().get();
}

void Renderer3dCommandBuffersImpl::dequeue(
	const bstone::Renderer3dCommandBufferPtr buffer)
{
	if (!buffer)
	{
		return;
	}

	static_cast<void>(std::remove_if(
		buffers_.begin(),
		buffers_.end(),
		[buffer](const auto& item)
		{
			return item.get() == buffer;
		}
	));
}

bstone::Renderer3dCommandBufferPtr Renderer3dCommandBuffersImpl::get(
	const int index)
{
	if (index < 0 || index >= get_count())
	{
		throw Renderer3dCommandBuffersImplException{"Index out of range."};
	}

	return buffers_[index].get();
}

Renderer3dCommandBuffers::Iterator Renderer3dCommandBuffersImpl::begin() noexcept
{
	return buffers_.data();
}

Renderer3dCommandBuffers::CIterator Renderer3dCommandBuffersImpl::cbegin() const noexcept
{
	return buffers_.data();
}

Renderer3dCommandBuffers::Iterator Renderer3dCommandBuffersImpl::end() noexcept
{
	return buffers_.data() + buffers_.size();
}

Renderer3dCommandBuffers::CIterator Renderer3dCommandBuffersImpl::cend() const noexcept
{
	return buffers_.data() + buffers_.size();
}

//
// Renderer3dCommandBuffersImpl
// ==========================================================================


// ==========================================================================
// Renderer3dCommandBuffersFactory
//

Renderer3dCommandBuffersUPtr Renderer3dCommandBuffersFactory::create()
{
	return std::make_unique<Renderer3dCommandBuffersImpl>();
}

//
// Renderer3dCommandBuffersFactory
// ==========================================================================


} // detail
} // bstone
