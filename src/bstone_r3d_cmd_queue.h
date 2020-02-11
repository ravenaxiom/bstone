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
// 3D-renderer's command queue.
//


#ifndef BSTONE_R3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_R3D_COMMAND_QUEUE_INCLUDED


#include "bstone_r3d_cmd_buffer.h"


namespace bstone
{


// ==========================================================================
// R3dCmdQueue
//

struct R3dCmdQueueEnqueueParam
{
	int initial_size_;
	int resize_delta_size_;
}; // R3dCmdQueueEnqueueParam

class R3dCmdQueue
{
protected:
	R3dCmdQueue() = default;


public:
	virtual ~R3dCmdQueue() = default;


	virtual int get_count() const noexcept = 0;

	virtual R3dCmdBufferPtr enqueue(
		const R3dCmdQueueEnqueueParam& param) = 0;

	virtual void dequeue(
		R3dCmdBufferPtr set) = 0;

	virtual R3dCmdBufferPtr get(
		const int index) = 0;
}; // R3dCmdQueue

using R3dCmdQueuePtr = R3dCmdQueue*;
using R3dCmdQueueUPtr = std::unique_ptr<R3dCmdQueue>;

//
// R3dCmdQueue
// ==========================================================================


// ==========================================================================
// R3dCmdQueueFactory
//

struct R3dCmdQueueFactory
{
	static R3dCmdQueueUPtr create();
}; // R3dCmdQueueFactory

//
// R3dCmdQueueFactory
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_COMMAND_QUEUE_INCLUDED
