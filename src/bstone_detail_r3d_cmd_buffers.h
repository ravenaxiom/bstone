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


#ifndef BSTONE_DETAIL_R3D_COMMAND_BUFFERS_INCLUDED
#define BSTONE_DETAIL_R3D_COMMAND_BUFFERS_INCLUDED


#include <memory>

#include "bstone_detail_r3d_cmd_buffer.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dCmdBuffers
//

class R3dCmdBuffers
{
protected:
	R3dCmdBuffers() = default;


public:
	using Iterator = R3dCmdBufferUPtr*;
	using CIterator = const R3dCmdBufferUPtr*;


	virtual ~R3dCmdBuffers() = default;


	virtual int get_count() const noexcept = 0;

	virtual bstone::R3dCmdBufferPtr enqueue(
		const R3dCmdQueueEnqueueParam& param) = 0;

	virtual void dequeue(
		bstone::R3dCmdBufferPtr buffer) = 0;

	virtual bstone::R3dCmdBufferPtr get(
		const int index) = 0;


	virtual Iterator begin() noexcept = 0;

	virtual CIterator cbegin() const noexcept = 0;

	virtual Iterator end() noexcept = 0;

	virtual CIterator cend() const noexcept = 0;
}; // R3dCmdBuffers

using R3dCmdBuffersPtr = R3dCmdBuffers*;
using R3dCmdBuffersUPtr = std::unique_ptr<R3dCmdBuffers>;

//
// R3dCmdBuffers
// ==========================================================================


// ==========================================================================
// R3dCmdBuffersFactory
//

struct R3dCmdBuffersFactory
{
	static R3dCmdBuffersUPtr create();
}; // R3dCmdBuffersFactory

//
// R3dCmdBuffersFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_COMMAND_BUFFERS_INCLUDED
