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


#ifndef BSTONE_DETAIL_R3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_DETAIL_R3D_COMMAND_QUEUE_INCLUDED


#include "bstone_detail_r3d_cmd_buffer.h"
#include "bstone_detail_r3d_cmd_buffers.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dCmdQueueImpl
//

class R3dCmdQueueImpl final :
	public R3dCmdQueue
{
public:
	R3dCmdQueueImpl();

	~R3dCmdQueueImpl() override;


	int get_count() const noexcept override;

	bstone::R3dCmdBufferPtr enqueue(
		const R3dCmdQueueEnqueueParam& param) override;

	void dequeue(
		bstone::R3dCmdBufferPtr buffer) override;

	bstone::R3dCmdBufferPtr get(
		const int index) override;


private:
	R3dCmdBuffersUPtr buffers_;


	static void validate_param(
		const R3dCmdQueueEnqueueParam& param);
}; // R3dCmdQueueImpl

using R3dCmdQueueImplPtr = R3dCmdQueueImpl*;
using R3dCmdQueueImplUPtr = std::unique_ptr<R3dCmdQueueImpl>;

//
// R3dCmdQueueImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_COMMAND_QUEUE_INCLUDED
