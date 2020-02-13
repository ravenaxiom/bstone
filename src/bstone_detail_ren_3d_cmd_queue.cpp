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

#include "bstone_detail_ren_3d_cmd_queue.h"

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dCmdQueueImpl
//

Ren3dCmdQueueImpl::Ren3dCmdQueueImpl()
	:
	buffers_{Ren3dCmdBuffersFactory::create()}
{
}

Ren3dCmdQueueImpl::~Ren3dCmdQueueImpl() = default;

int Ren3dCmdQueueImpl::get_count() const noexcept
{
	return buffers_->get_count();
}

bstone::Ren3dCmdBufferPtr Ren3dCmdQueueImpl::enqueue(
	const Ren3dCreateCmdBufferParam& param)
{
	return buffers_->enqueue(param);
}

void Ren3dCmdQueueImpl::dequeue(
	bstone::Ren3dCmdBufferPtr buffer)
{
	buffers_->dequeue(buffer);
}

bstone::Ren3dCmdBufferPtr Ren3dCmdQueueImpl::get(
	const int index)
{
	return buffers_->get(index);
}

void Ren3dCmdQueueImpl::validate_param(
	const Ren3dCreateCmdBufferParam& param)
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
// Ren3dCmdQueueImpl
// ==========================================================================


} // detail


// ==========================================================================
// Ren3dCmdQueueFactory
//

Ren3dCmdQueueUPtr Ren3dCmdQueueFactory::create()
{
	return std::make_unique<detail::Ren3dCmdQueueImpl>();
}

//
// Ren3dCmdQueueFactory
// ==========================================================================


} // bstone
