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
// OpenGL vertex array object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_REN_3D_GL_VAO_MGR_INCLUDED
#define BSTONE_DETAIL_REN_3D_GL_VAO_MGR_INCLUDED


#include <memory>


namespace bstone
{


struct Ren3dDeviceFeatures;

class Ren3dBuffer;
using Ren3dBufferPtr = Ren3dBuffer*;


namespace detail
{


class Ren3dGlContext;
using Ren3dGlContextPtr = Ren3dGlContext*;

struct Ren3dGlDeviceFeatures;

class Ren3dGlVao;
using Ren3dGlVaoPtr = Ren3dGlVao*;


// ==========================================================================
// Ren3dGlVaoMgr
//

class Ren3dGlVaoMgr
{
protected:
	Ren3dGlVaoMgr() = default;


public:
	virtual ~Ren3dGlVaoMgr() = default;


	virtual Ren3dGlContextPtr get_context() const noexcept = 0;

	virtual Ren3dGlVaoPtr create() = 0;

	virtual void destroy(
		const Ren3dGlVaoPtr vao) = 0;

	virtual void push_current_set_default() = 0;

	virtual void pop() = 0;

	virtual void bind(
		const Ren3dGlVaoPtr vao) = 0;

	virtual bool set_current_index_buffer(
		const Ren3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // Ren3dGlVaoMgr

using Ren3dGlVaoMgrPtr = Ren3dGlVaoMgr*;
using Ren3dGlVaoMgrUPtr = std::unique_ptr<Ren3dGlVaoMgr>;

//
// Ren3dGlVaoMgr
// ==========================================================================


// ==========================================================================
// Ren3dGlVaoMgrFactory
//

struct Ren3dGlVaoMgrFactory
{
	static Ren3dGlVaoMgrUPtr create(
		const Ren3dGlContextPtr gl_context,
		const Ren3dDeviceFeatures& device_features,
		const Ren3dGlDeviceFeatures& gl_device_features);
}; // Ren3dGlVaoMgrFactory

//
// Ren3dGlVaoMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_REN_3D_GL_VAO_MGR_INCLUDED
