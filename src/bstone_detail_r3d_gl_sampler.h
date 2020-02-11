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


#ifndef BSTONE_DETAIL_R3D_GL_SAMPLER_INCLUDED
#define BSTONE_DETAIL_R3D_GL_SAMPLER_INCLUDED


#include "bstone_r3d_sampler.h"


namespace bstone
{
namespace detail
{


class R3dGlContext;
using R3dGlContextPtr = R3dGlContext*;


// =========================================================================
// R3dGlSampler
//

class R3dGlSampler :
	public R3dSampler
{
protected:
	R3dGlSampler() = default;


public:
	~R3dGlSampler() override = default;


	virtual void set() = 0;
}; // R3dGlSampler

using R3dGlSamplerPtr = R3dGlSampler*;
using R3dGlSamplerUPtr = std::unique_ptr<R3dGlSampler>;

//
// R3dGlSampler
// =========================================================================


// =========================================================================
// R3dGlSamplerFactory
//

struct R3dGlSamplerFactory
{
	static R3dGlSamplerUPtr create(
		R3dGlContextPtr gl_context,
		const R3dSamplerCreateParam& param);
}; // R3dGlSamplerFactory

//
// R3dGlSamplerFactory
// =========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_SAMPLER_INCLUDED
