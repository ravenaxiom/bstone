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


#ifndef BSTONE_DETAIL_R3D_GL_VAO_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_VAO_MGR_INCLUDED


#include <memory>


namespace bstone
{


struct R3dDeviceFeatures;

class R3dBuffer;
using R3dBufferPtr = R3dBuffer*;


namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;

struct R3dGlDeviceFeatures;

class R3dGlVao;
using R3dGlVaoPtr = R3dGlVao*;


// ==========================================================================
// R3dGlVaoMgr
//

class R3dGlVaoMgr
{
protected:
	R3dGlVaoMgr() = default;


public:
	virtual ~R3dGlVaoMgr() = default;


	virtual R3dGlContextPtr get_context() const noexcept = 0;

	virtual R3dGlVaoPtr create() = 0;

	virtual void destroy(
		const R3dGlVaoPtr vao) = 0;

	virtual void push_current_set_default() = 0;

	virtual void pop() = 0;

	virtual void bind(
		const R3dGlVaoPtr vao) = 0;

	virtual bool set_current_index_buffer(
		const R3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // R3dGlVaoMgr

using R3dGlVaoMgrPtr = R3dGlVaoMgr*;
using GlVaoMgrUPtr = std::unique_ptr<R3dGlVaoMgr>;

//
// R3dGlVaoMgr
// ==========================================================================


// ==========================================================================
// GlVaoMgrFactory
//

struct GlVaoMgrFactory
{
	static GlVaoMgrUPtr create(
		const R3dGlContextPtr gl_context,
		const R3dDeviceFeatures& device_features,
		const R3dGlDeviceFeatures& gl_device_features);
}; // GlVaoMgrFactory

//
// GlVaoMgrFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_VAO_MGR_INCLUDED
