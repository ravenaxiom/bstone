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
// 3D renderer's sampler.
//


#ifndef BSTONE_RENDERER_3D_SAMPLER_INCLUDED
#define BSTONE_RENDERER_3D_SAMPLER_INCLUDED


#include <memory>

#include "bstone_renderer_3d_types.h"


namespace bstone
{


// ==========================================================================
// Renderer3dSampler
//

struct Renderer3dSamplerState
{
	Renderer3dFilterKind mag_filter_;
	Renderer3dFilterKind min_filter_;

	Renderer3dMipmapMode mipmap_mode_;

	Renderer3dAddressMode address_mode_u_;
	Renderer3dAddressMode address_mode_v_;

	int anisotropy_;
}; // Renderer3dSamplerState

struct Renderer3dSamplerCreateParam
{
	Renderer3dSamplerState state_;
}; // Renderer3dSamplerCreateParam

struct Renderer3dSamplerUpdateParam
{
	Renderer3dSamplerState state_;
}; // Renderer3dSamplerUpdateParam


class Renderer3dSampler
{
protected:
	Renderer3dSampler() = default;


public:
	virtual ~Renderer3dSampler() = default;


	virtual void update(
		const Renderer3dSamplerUpdateParam& param) = 0;

	virtual const Renderer3dSamplerState& get_state() const noexcept = 0;
}; // Renderer3dSampler

using Renderer3dSamplerPtr = Renderer3dSampler*;
using Renderer3dSamplerUPtr = std::unique_ptr<Renderer3dSampler>;

//
// Renderer3dSampler
// ==========================================================================


} // bstone


#endif // !BSTONE_RENDERER_3D_SAMPLER_INCLUDED
