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


#ifndef BSTONE_DETAIL_REN_3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_DETAIL_REN_3D_COMMAND_QUEUE_INCLUDED


#include "bstone_detail_ren_3d_cmd_buffer.h"
#include "bstone_detail_ren_3d_cmd_buffers.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dCmdQueueImpl
//

class Ren3dCmdQueueImpl final :
	public Ren3dCmdQueue
{
public:
	Ren3dCmdQueueImpl();

	~Ren3dCmdQueueImpl() override;


	int get_count() const noexcept override;

	bstone::Ren3dCmdBufferPtr enqueue(
		const Ren3dCmdQueueEnqueueParam& param) override;

	void dequeue(
		bstone::Ren3dCmdBufferPtr buffer) override;

	bstone::Ren3dCmdBufferPtr get(
		const int index) override;


private:
	Ren3dCmdBuffersUPtr buffers_;


	static void validate_param(
		const Ren3dCmdQueueEnqueueParam& param);
}; // Ren3dCmdQueueImpl

using Ren3dCmdQueueImplPtr = Ren3dCmdQueueImpl*;
using Ren3dCmdQueueImplUPtr = std::unique_ptr<Ren3dCmdQueueImpl>;

//
// Ren3dCmdQueueImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_COMMAND_QUEUE_INCLUDED
