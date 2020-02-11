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
// R3dCmdBuffersImplException
//

class R3dCmdBuffersImplException :
	public Exception
{
public:
	explicit R3dCmdBuffersImplException(
		const char* const message)
		:
		Exception{std::string{"[R3D_CMD_BUFS] "} + message}
	{
	}
}; // R3dCmdBuffersImplException

//
// R3dCmdBuffersImplException
// ==========================================================================


// ==========================================================================
// R3dCmdBuffersImpl
//

class R3dCmdBuffersImpl final :
	public R3dCmdBuffers
{
public:
	R3dCmdBuffersImpl();

	~R3dCmdBuffersImpl() override = default;


	int get_count() const noexcept override;

	bstone::R3dCmdBufferPtr enqueue(
		const R3dCmdQueueEnqueueParam& param) override;

	void dequeue(
		const bstone::R3dCmdBufferPtr buffer) override;

	bstone::R3dCmdBufferPtr get(
		const int index) override;


	Iterator begin() noexcept override;

	CIterator cbegin() const noexcept override;

	Iterator end() noexcept override;

	CIterator cend() const noexcept override;


private:
	static constexpr auto reserve_count = 16;


	using Buffers = std::vector<R3dCmdBufferUPtr>;
	Buffers buffers_;
}; // R3dCmdBuffersImpl

using R3dCmdBuffersImplPtr = R3dCmdBuffersImpl*;
using R3dCmdBuffersImplUPtr = std::unique_ptr<R3dCmdBuffersImpl>;

//
// R3dCmdBuffersImpl
// ==========================================================================


// ==========================================================================
// R3dCmdBuffersImpl
//

R3dCmdBuffersImpl::R3dCmdBuffersImpl()
	:
	buffers_{}
{
	buffers_.reserve(reserve_count);
}

int R3dCmdBuffersImpl::get_count() const noexcept
{
	return static_cast<int>(buffers_.size());
}

bstone::R3dCmdBufferPtr R3dCmdBuffersImpl::enqueue(
	const R3dCmdQueueEnqueueParam& param)
{
	auto buffer = std::make_unique<R3dCmdBufferImpl>(param);

	buffers_.push_back(std::move(buffer));

	return buffers_.back().get();
}

void R3dCmdBuffersImpl::dequeue(
	const bstone::R3dCmdBufferPtr buffer)
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

bstone::R3dCmdBufferPtr R3dCmdBuffersImpl::get(
	const int index)
{
	if (index < 0 || index >= get_count())
	{
		throw R3dCmdBuffersImplException{"Index out of range."};
	}

	return buffers_[index].get();
}

R3dCmdBuffers::Iterator R3dCmdBuffersImpl::begin() noexcept
{
	return buffers_.data();
}

R3dCmdBuffers::CIterator R3dCmdBuffersImpl::cbegin() const noexcept
{
	return buffers_.data();
}

R3dCmdBuffers::Iterator R3dCmdBuffersImpl::end() noexcept
{
	return buffers_.data() + buffers_.size();
}

R3dCmdBuffers::CIterator R3dCmdBuffersImpl::cend() const noexcept
{
	return buffers_.data() + buffers_.size();
}

//
// R3dCmdBuffersImpl
// ==========================================================================


// ==========================================================================
// R3dCmdBuffersFactory
//

R3dCmdBuffersUPtr R3dCmdBuffersFactory::create()
{
	return std::make_unique<R3dCmdBuffersImpl>();
}

//
// R3dCmdBuffersFactory
// ==========================================================================


} // detail
} // bstone
