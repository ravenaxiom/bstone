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


#ifndef BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED
#define BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED


#include "bstone_detail_renderer_3d_command_buffer.h"
#include "bstone_detail_renderer_3d_command_buffers.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandQueueImpl
//

class Renderer3dCommandQueueImpl final :
	public Renderer3dCommandQueue
{
public:
	Renderer3dCommandQueueImpl();

	~Renderer3dCommandQueueImpl() override;


	int get_count() const noexcept override;

	bstone::Renderer3dCommandBufferPtr enqueue(
		const Renderer3dCommandQueueEnqueueParam& param) override;

	void dequeue(
		bstone::Renderer3dCommandBufferPtr buffer) override;

	bstone::Renderer3dCommandBufferPtr get(
		const int index) override;


private:
	Renderer3dCommandBuffersUPtr buffers_;


	static void validate_param(
		const Renderer3dCommandQueueEnqueueParam& param);
}; // Renderer3dCommandQueueImpl

using Renderer3dCommandQueueImplPtr = Renderer3dCommandQueueImpl*;
using Renderer3dCommandQueueImplUPtr = std::unique_ptr<Renderer3dCommandQueueImpl>;

//
// Renderer3dCommandQueueImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_RENDERER_3D_COMMAND_QUEUE_INCLUDED
