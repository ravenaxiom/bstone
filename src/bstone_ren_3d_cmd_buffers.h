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
// 3D renderer's command buffer list.
//


#ifndef BSTONE_DETAIL_REN_3D_COMMAND_BUFFERS_INCLUDED
#define BSTONE_DETAIL_REN_3D_COMMAND_BUFFERS_INCLUDED


#include <memory>

#include "bstone_ren_3d_cmd_buffer.h"


namespace bstone
{


// ==========================================================================
// Ren3dCmdBuffers
//

class Ren3dCmdBuffers
{
protected:
	Ren3dCmdBuffers() = default;


public:
	virtual ~Ren3dCmdBuffers() = default;


	virtual int get_count() const noexcept = 0;

	virtual Ren3dCmdBufferPtr add(
		const Ren3dCreateCmdBufferParam& param) = 0;

	virtual void remove(
		Ren3dCmdBufferPtr buffer) = 0;

	virtual Ren3dCmdBufferPtr get(
		const int index) = 0;
}; // Ren3dCmdBuffers

using Ren3dCmdBuffersPtr = Ren3dCmdBuffers*;
using Ren3dCmdBuffersUPtr = std::unique_ptr<Ren3dCmdBuffers>;

//
// Ren3dCmdBuffers
// ==========================================================================


// ==========================================================================
// Ren3dCmdBuffersFactory
//

struct Ren3dCmdBuffersFactory
{
	static Ren3dCmdBuffersUPtr create();
}; // Ren3dCmdBuffersFactory

//
// Ren3dCmdBuffersFactory
// ==========================================================================


} // bstone


#endif // !BSTONE_DETAIL_REN_3D_COMMAND_BUFFERS_INCLUDED
