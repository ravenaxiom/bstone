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
// OpenGL sampler object manager (implementation interface).
//


#ifndef BSTONE_DETAIL_R3D_GL_SAMPLER_MGR_INCLUDED
#define BSTONE_DETAIL_R3D_GL_SAMPLER_MGR_INCLUDED


#include <memory>

#include "bstone_r3d_sampler.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// ==========================================================================
// GlSamplerManager
//

class GlSamplerManager
{
protected:
	GlSamplerManager() = default;


public:
	virtual ~GlSamplerManager() = default;


	virtual Renderer3dSamplerUPtr create(
		const Renderer3dSamplerCreateParam& param) = 0;

	virtual void notify_destroy(
		const Renderer3dSamplerPtr sampler) noexcept = 0;

	virtual void set(
		const Renderer3dSamplerPtr sampler) = 0;

	virtual const Renderer3dSamplerState& get_current_state() const noexcept = 0;
}; // GlSamplerManager

using GlSamplerManagerPtr = GlSamplerManager*;
using GlSamplerManagerUPtr = std::unique_ptr<GlSamplerManager>;

//
// GlSamplerManager
// ==========================================================================


// ==========================================================================
// GlSamplerManagerFactory
//

struct GlSamplerManagerFactory
{
	static GlSamplerManagerUPtr create(
		const GlContextPtr gl_context);
}; // GlSamplerManagerFactory

//
// GlSamplerManagerFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_SAMPLER_MGR_INCLUDED
