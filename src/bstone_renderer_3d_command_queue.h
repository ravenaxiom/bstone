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


#ifndef BSTONE_RENDERER_3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_RENDERER_3D_COMMAND_QUEUE_INCLUDED


#include "bstone_renderer_3d_command_buffer.h"


namespace bstone
{


// ==========================================================================
// Renderer3dCommandQueue
//

struct Renderer3dCommandQueueEnqueueParam
{
	int initial_size_;
	int resize_delta_size_;
}; // Renderer3dCommandQueueEnqueueParam

class Renderer3dCommandQueue
{
protected:
	Renderer3dCommandQueue() = default;


public:
	virtual ~Renderer3dCommandQueue() = default;


	virtual int get_count() const noexcept = 0;

	virtual Renderer3dCommandBufferPtr enqueue(
		const Renderer3dCommandQueueEnqueueParam& param) = 0;

	virtual void dequeue(
		Renderer3dCommandBufferPtr set) = 0;

	virtual Renderer3dCommandBufferPtr get(
		const int index) = 0;
}; // Renderer3dCommandQueue

using Renderer3dCommandQueuePtr = Renderer3dCommandQueue*;
using Renderer3dCommandQueueUPtr = std::unique_ptr<Renderer3dCommandQueue>;

//
// Renderer3dCommandQueue
// ==========================================================================


// ==========================================================================
// Renderer3dCommandQueueFactory
//

struct Renderer3dCommandQueueFactory
{
	static Renderer3dCommandQueueUPtr create();
}; // Renderer3dCommandQueueFactory

//
// Renderer3dCommandQueueFactory
// ==========================================================================


} // bstone


#endif // !BSTONE_RENDERER_3D_COMMAND_QUEUE_INCLUDED
