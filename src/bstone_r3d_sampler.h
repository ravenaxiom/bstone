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


#ifndef BSTONE_R3D_SAMPLER_INCLUDED
#define BSTONE_R3D_SAMPLER_INCLUDED


#include <memory>

#include "bstone_r3d_types.h"


namespace bstone
{


// ==========================================================================
// R3dSampler
//

struct R3dSamplerState
{
	R3dFilterKind mag_filter_;
	R3dFilterKind min_filter_;

	R3dMipmapMode mipmap_mode_;

	R3dAddressMode address_mode_u_;
	R3dAddressMode address_mode_v_;

	int anisotropy_;
}; // R3dSamplerState

struct R3dSamplerCreateParam
{
	R3dSamplerState state_;
}; // R3dSamplerCreateParam

struct R3dSamplerUpdateParam
{
	R3dSamplerState state_;
}; // R3dSamplerUpdateParam


class R3dSampler
{
protected:
	R3dSampler() = default;


public:
	virtual ~R3dSampler() = default;


	virtual void update(
		const R3dSamplerUpdateParam& param) = 0;

	virtual const R3dSamplerState& get_state() const noexcept = 0;
}; // R3dSampler

using R3dSamplerPtr = R3dSampler*;
using R3dSamplerUPtr = std::unique_ptr<R3dSampler>;

//
// R3dSampler
// ==========================================================================


} // bstone


#endif // !BSTONE_R3D_SAMPLER_INCLUDED
