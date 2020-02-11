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
// 3D renderer's command queue (implementaion).
//


#include "bstone_precompiled.h"

#include "bstone_detail_r3d_cmd_queue.h"

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dCmdQueueImpl
//

R3dCmdQueueImpl::R3dCmdQueueImpl()
	:
	buffers_{R3dCmdBuffersFactory::create()}
{
}

R3dCmdQueueImpl::~R3dCmdQueueImpl() = default;

int R3dCmdQueueImpl::get_count() const noexcept
{
	return buffers_->get_count();
}

bstone::R3dCmdBufferPtr R3dCmdQueueImpl::enqueue(
	const R3dCmdQueueEnqueueParam& param)
{
	return buffers_->enqueue(param);
}

void R3dCmdQueueImpl::dequeue(
	bstone::R3dCmdBufferPtr buffer)
{
	buffers_->dequeue(buffer);
}

bstone::R3dCmdBufferPtr R3dCmdQueueImpl::get(
	const int index)
{
	return buffers_->get(index);
}

void R3dCmdQueueImpl::validate_param(
	const R3dCmdQueueEnqueueParam& param)
{
	if (param.initial_size_ < 0)
	{
		throw Exception{"Initial size out of range."};
	}

	if (param.resize_delta_size_ < 0)
	{
		throw Exception{"Resize delta out of range."};
	}
}

//
// R3dCmdQueueImpl
// ==========================================================================


} // detail


// ==========================================================================
// R3dCmdQueueFactory
//

R3dCmdQueueUPtr R3dCmdQueueFactory::create()
{
	return std::make_unique<detail::R3dCmdQueueImpl>();
}

//
// R3dCmdQueueFactory
// ==========================================================================


} // bstone
