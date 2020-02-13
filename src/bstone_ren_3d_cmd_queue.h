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


#ifndef BSTONE_REN_3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_REN_3D_COMMAND_QUEUE_INCLUDED


#include "bstone_ren_3d_cmd_buffer.h"


namespace bstone
{


// ==========================================================================
// Ren3dCmdQueue
//

class Ren3dCmdQueue
{
protected:
	Ren3dCmdQueue() = default;


public:
	virtual ~Ren3dCmdQueue() = default;


	virtual int get_count() const noexcept = 0;

	virtual Ren3dCmdBufferPtr enqueue(
		const Ren3dCreateCmdBufferParam& param) = 0;

	virtual void dequeue(
		Ren3dCmdBufferPtr command_buffer) = 0;

	virtual Ren3dCmdBufferPtr get(
		const int index) = 0;
}; // Ren3dCmdQueue

using Ren3dCmdQueuePtr = Ren3dCmdQueue*;
using Ren3dCmdQueueUPtr = std::unique_ptr<Ren3dCmdQueue>;

//
// Ren3dCmdQueue
// ==========================================================================


// ==========================================================================
// Ren3dCmdQueueFactory
//

struct Ren3dCmdQueueFactory
{
	static Ren3dCmdQueueUPtr create();
}; // Ren3dCmdQueueFactory

//
// Ren3dCmdQueueFactory
// ==========================================================================


} // bstone


#endif // !BSTONE_REN_3D_COMMAND_QUEUE_INCLUDED
