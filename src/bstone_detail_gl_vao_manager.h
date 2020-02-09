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


#ifndef BSTONE_DETAIL_GL_VAO_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_VAO_MANAGER_INCLUDED


#include <memory>


namespace bstone
{


struct Renderer3dDeviceFeatures;

class Renderer3dBuffer;
using Renderer3dBufferPtr = Renderer3dBuffer*;


namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;

struct GlDeviceFeatures;

class GlVao;
using GlVaoPtr = GlVao*;


// ==========================================================================
// GlVaoManager
//

class GlVaoManager
{
protected:
	GlVaoManager() = default;


public:
	virtual ~GlVaoManager() = default;


	virtual GlContextPtr get_context() const noexcept = 0;

	virtual GlVaoPtr create() = 0;

	virtual void destroy(
		const GlVaoPtr vao) = 0;

	virtual void push_current_set_default() = 0;

	virtual void pop() = 0;

	virtual void bind(
		const GlVaoPtr vao) = 0;

	virtual bool set_current_index_buffer(
		const Renderer3dBufferPtr index_buffer) = 0;


	virtual void enable_location(
		const int location,
		const bool is_enable) = 0;
}; // GlVaoManager

using GlVaoManagerPtr = GlVaoManager*;
using GlVaoManagerUPtr = std::unique_ptr<GlVaoManager>;

//
// GlVaoManager
// ==========================================================================


// ==========================================================================
// GlVaoManagerFactory
//

struct GlVaoManagerFactory
{
	static GlVaoManagerUPtr create(
		const GlContextPtr gl_context,
		const Renderer3dDeviceFeatures& device_features,
		const GlDeviceFeatures& gl_device_features);
}; // GlVaoManagerFactory

//
// GlVaoManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_VAO_MANAGER_INCLUDED
