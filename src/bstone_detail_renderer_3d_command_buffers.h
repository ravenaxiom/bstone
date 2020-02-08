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


#ifndef BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFERS_INCLUDED
#define BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFERS_INCLUDED


#include <memory>

#include "bstone_detail_renderer_3d_command_buffer.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandBuffers
//

class Renderer3dCommandBuffers
{
protected:
	Renderer3dCommandBuffers() = default;


public:
	using Iterator = Renderer3dCommandBufferUPtr*;
	using CIterator = const Renderer3dCommandBufferUPtr*;


	virtual ~Renderer3dCommandBuffers() = default;


	virtual int get_count() const noexcept = 0;

	virtual bstone::Renderer3dCommandBufferPtr enqueue(
		const Renderer3dCommandQueueEnqueueParam& param) = 0;

	virtual void dequeue(
		bstone::Renderer3dCommandBufferPtr buffer) = 0;

	virtual bstone::Renderer3dCommandBufferPtr get(
		const int index) = 0;


	virtual Iterator begin() noexcept = 0;

	virtual CIterator cbegin() const noexcept = 0;

	virtual Iterator end() noexcept = 0;

	virtual CIterator cend() const noexcept = 0;
}; // Renderer3dCommandBuffers

using Renderer3dCommandBuffersPtr = Renderer3dCommandBuffers*;
using Renderer3dCommandBuffersUPtr = std::unique_ptr<Renderer3dCommandBuffers>;

//
// Renderer3dCommandBuffers
// ==========================================================================


// ==========================================================================
// Renderer3dCommandBuffersFactory
//

struct Renderer3dCommandBuffersFactory
{
	static Renderer3dCommandBuffersUPtr create();
}; // Renderer3dCommandBuffersFactory

//
// Renderer3dCommandBuffersFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_RENDERER_3D_COMMAND_BUFFERS_INCLUDED
