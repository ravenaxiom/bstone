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
// OpenGL sampler.
//
// !!! Internal usage only !!!
//


#ifndef BSTONE_DETAIL_GL_SAMPLER_INCLUDED
#define BSTONE_DETAIL_GL_SAMPLER_INCLUDED


#include "bstone_renderer_3d_sampler.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// =========================================================================
// GlSampler
//

class GlSampler :
	public Renderer3dSampler
{
protected:
	GlSampler();


public:
	~GlSampler() override;


	virtual void bind() = 0;
}; // GlSampler

using GlSamplerPtr = GlSampler*;
using GlSamplerUPtr = std::unique_ptr<GlSampler>;

//
// GlSampler
// =========================================================================


// =========================================================================
// GlSamplerFactory
//

struct GlSamplerFactory
{
	static GlSamplerUPtr create(
		GlContextPtr gl_context,
		const Renderer3dSamplerCreateParam& param);
}; // GlSamplerFactory

//
// GlSamplerFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_SAMPLER_INCLUDED
