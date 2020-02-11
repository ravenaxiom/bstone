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
// Texture manager (hardware).
//


#ifndef BSTONE_HW_TEXTURE_MGR_INCLUDED
#define BSTONE_HW_TEXTURE_MGR_INCLUDED


#include <memory>

#include "bstone_r3d.h"
#include "bstone_rgb_palette.h"


namespace bstone
{


class SpriteCache;
using SpriteCachePtr = SpriteCache*;

class MtTaskMgr;
using MtTaskMgrPtr = MtTaskMgr*;


enum class HwTextureMgrSolid1x1Id
{
	black,
	white,
	fade_2d,
	fade_3d,
	flooring,
	ceiling,

	// Item count.
	count_,
}; // HwTextureMgrSolid1x1Id

enum class HwTextureMgrUpscaleFilterKind
{
	none,
	xbrz,
}; // HwTextureMgrUpscaleFilterKind


class HwTextureMgr
{
protected:
	HwTextureMgr() = default;


public:
	virtual ~HwTextureMgr() = default;


	virtual int upscale_filter_get_min_factor(
		const HwTextureMgrUpscaleFilterKind upscale_filter_kind) const = 0;

	virtual int upscale_filter_get_max_factor(
		const HwTextureMgrUpscaleFilterKind upscale_filter_kind) const = 0;

	virtual HwTextureMgrUpscaleFilterKind upscale_filter_get_kind() const noexcept = 0;

	virtual int upscale_filter_get_factor() const noexcept = 0;

	virtual void upscale_filter_set(
		const HwTextureMgrUpscaleFilterKind upscale_filter_kind,
		const int upscale_filter_factor) = 0;


	virtual void cache_begin() = 0;

	virtual void cache_end() = 0;

	virtual void cache_purge() = 0;


	virtual void wall_cache(
		const int id) = 0;

	virtual Ren3dTexture2dPtr wall_get(
		const int id) const = 0;


	virtual void sprite_cache(
		const int id) = 0;

	virtual Ren3dTexture2dPtr sprite_get(
		const int id) const = 0;


	virtual void ui_destroy() = 0;

	virtual void ui_create(
		const std::uint8_t* const indexed_pixels,
		const bool* const indexed_alphas,
		const Rgba8PaletteCPtr indexed_palette) = 0;

	virtual void ui_update() = 0;

	virtual Ren3dTexture2dPtr ui_get() const = 0;


	virtual void solid_1x1_destroy(
		const HwTextureMgrSolid1x1Id id) = 0;

	virtual void solid_1x1_create(
		const HwTextureMgrSolid1x1Id id) = 0;

	virtual void solid_1x1_update(
		const HwTextureMgrSolid1x1Id id,
		const Rgba8 color) = 0;

	virtual Ren3dTexture2dPtr solid_1x1_get(
		const HwTextureMgrSolid1x1Id id) const = 0;
}; // HwTextureMgr

using HwTextureMgrPtr = HwTextureMgr*;
using HwTextureMgrUPtr = std::unique_ptr<HwTextureMgr>;


class HwTextureMgrFactory
{
public:
	static HwTextureMgrUPtr create(
		const Ren3dPtr renderer,
		const SpriteCachePtr sprite_cache,
		const MtTaskMgrPtr mt_task_manager);
}; // HwTextureMgrFactory


} // bstone


#endif // !BSTONE_HW_TEXTURE_MGR_INCLUDED
