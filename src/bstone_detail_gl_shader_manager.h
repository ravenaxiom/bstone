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
// OpenGL shader object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_GL_SHADER_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_SHADER_MANAGER_INCLUDED


#include <memory>

#include "bstone_renderer_3d.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// ==========================================================================
// GlShaderManager
//

class GlShaderManager
{
protected:
	GlShaderManager();


public:
	virtual ~GlShaderManager();


	virtual Renderer3dShaderUPtr create(
		const Renderer3dShaderCreateParam& param) = 0;

	virtual void notify_destroy(
		const Renderer3dShaderPtr shader) noexcept = 0;
}; // GlShaderManager

using GlShaderManagerPtr = GlShaderManager*;
using GlShaderManagerUPtr = std::unique_ptr<GlShaderManager>;

//
// GlShaderManager
// ==========================================================================


// ==========================================================================
// GlShaderManagerFactory
//

struct GlShaderManagerFactory
{
	static GlShaderManagerUPtr create(
		const GlContextPtr gl_context);
}; // GlShaderManagerFactory

//
// GlShaderManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_SHADER_MANAGER_INCLUDED
