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
// 3D renderer's buffer.
//


#ifndef BSTONE_RENDERER_3D_BUFFER_INCLUDED
#define BSTONE_RENDERER_3D_BUFFER_INCLUDED


#include <memory>


namespace bstone
{


// ==========================================================================
// Renderer3dBuffer
//

enum class Renderer3dBufferKind
{
	none,
	index,
	vertex,
}; // Renderer3dBufferKind

enum class Renderer3dBufferUsageKind
{
	none,

	// Target - drawing; updates - continuous.
	draw_streaming,

	// Target - drawing; updates - seldom.
	draw_static,

	// Target - drawing; updates - often.
	draw_dynamic,
}; // Renderer3dBufferUsageKind

struct Renderer3dBufferCreateParam
{
	Renderer3dBufferKind kind_;
	Renderer3dBufferUsageKind usage_kind_;
	int size_;
}; // Renderer3dBufferCreateParam

struct Renderer3dBufferUpdateParam
{
	int offset_;
	int size_;
	const void* data_;
}; // Renderer3dBufferUpdateParam


class Renderer3dBuffer
{
protected:
	Renderer3dBuffer() = default;


public:
	virtual ~Renderer3dBuffer() = default;


	virtual Renderer3dBufferKind get_kind() const noexcept = 0;

	virtual Renderer3dBufferUsageKind get_usage_kind() const noexcept = 0;

	virtual int get_size() const noexcept = 0;

	virtual void update(
		const Renderer3dBufferUpdateParam& param) = 0;
}; // Renderer3dBuffer

using Renderer3dBufferPtr = Renderer3dBuffer*;
using Renderer3dBufferUPtr = std::unique_ptr<Renderer3dBuffer>;

//
// Renderer3dBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_RENDERER_3D_BUFFER_INCLUDED
