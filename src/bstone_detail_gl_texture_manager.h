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
// OpenGL texture object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_GL_TEXTURE_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_TEXTURE_MANAGER_INCLUDED


#include <memory>

#include "bstone_renderer_3d_sampler.h"
#include "bstone_renderer_3d_texture_2d.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// ==========================================================================
// GlTextureManager
//

class GlTextureManager
{
protected:
	GlTextureManager();


public:
	virtual ~GlTextureManager();


	virtual GlContextPtr get_gl_context() const noexcept = 0;


	virtual Renderer3dTexture2dUPtr create(
		const Renderer3dTexture2dCreateParam& param) = 0;

	virtual void notify_destroy(
		const Renderer3dTexture2dPtr texture_2d) noexcept = 0;

	virtual void set(
		const Renderer3dTexture2dPtr texture_2d) = 0;

	virtual bool set_current(
		const Renderer3dTexture2dPtr texture_2d) = 0;

	virtual void update_current_sampler_state(
		const Renderer3dSamplerState& sampler_state) = 0;
}; // GlTextureManager

using GlTextureManagerPtr = GlTextureManager*;
using GlTextureManagerUPtr = std::unique_ptr<GlTextureManager>;

//
// GlTextureManager
// ==========================================================================


// ==========================================================================
// GlTextureManagerFactory
//

struct GlTextureManagerFactory
{
	static GlTextureManagerUPtr create(
		const GlContextPtr gl_context);
}; // GlTextureManagerFactory

//
// GlTextureManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_TEXTURE_MANAGER_INCLUDED
