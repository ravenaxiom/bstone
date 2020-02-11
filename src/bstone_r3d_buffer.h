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


#ifndef BSTONE_R3D_BUFFER_INCLUDED
#define BSTONE_R3D_BUFFER_INCLUDED


#include <memory>


namespace bstone
{


// ==========================================================================
// R3dBuffer
//

enum class R3dBufferKind
{
	none,
	index,
	vertex,
}; // R3dBufferKind

enum class R3dBufferUsageKind
{
	none,

	// Target - drawing; updates - continuous.
	draw_streaming,

	// Target - drawing; updates - seldom.
	draw_static,

	// Target - drawing; updates - often.
	draw_dynamic,
}; // R3dBufferUsageKind

struct R3dBufferCreateParam
{
	R3dBufferKind kind_;
	R3dBufferUsageKind usage_kind_;
	int size_;
}; // R3dBufferCreateParam

struct R3dBufferUpdateParam
{
	int offset_;
	int size_;
	const void* data_;
}; // R3dBufferUpdateParam


class R3dBuffer
{
protected:
	R3dBuffer() = default;


public:
	virtual ~R3dBuffer() = default;


	virtual R3dBufferKind get_kind() const noexcept = 0;

	virtual R3dBufferUsageKind get_usage_kind() const noexcept = 0;

	virtual int get_size() const noexcept = 0;

	virtual void update(
		const R3dBufferUpdateParam& param) = 0;
}; // R3dBuffer

using R3dBufferPtr = R3dBuffer*;
using R3dBufferUPtr = std::unique_ptr<R3dBuffer>;

//
// R3dBuffer
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_BUFFER_INCLUDED
