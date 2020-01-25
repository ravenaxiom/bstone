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
// OpenGL extension manager (implementation).
//
// !!! Internal usage only !!!
//


#ifndef BSTONE_DETAIL_GL_EXTENSION_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_EXTENSION_MANAGER_INCLUDED


#include <memory>
#include <string>


namespace bstone
{
namespace detail
{


enum class GlExtensionId
{
	// Virtual extensions.
	//

	// glGetIntegerv.
	essentials,

	// OpenGL 2.0.
	v2_0,

	// OpenGL 3.2 (core).
	v3_2_core,

	// OpenGL ES 2.0
	es_v2_0,


	// Real extensions.
	//

	arb_buffer_storage,
	arb_direct_state_access,
	arb_framebuffer_object,
	arb_sampler_objects,
	arb_separate_shader_objects,
	arb_texture_filter_anisotropic,
	arb_texture_non_power_of_two,
	arb_vertex_array_object,

	ext_framebuffer_blit,
	ext_framebuffer_multisample,
	ext_framebuffer_object,
	ext_packed_depth_stencil,
	ext_texture_filter_anisotropic,

	oes_texture_npot,


	// Item count.
	count_,
}; // GlExtensionId


class GlExtensionManager
{
protected:
	GlExtensionManager();


public:
	virtual ~GlExtensionManager();


	virtual int get_count() const noexcept = 0;

	virtual const std::string& get_name(
		const int extension_index) const noexcept = 0;


	virtual void probe(
		const GlExtensionId extension_id) = 0;


	virtual bool has(
		const GlExtensionId extension_id) const noexcept = 0;

	virtual bool operator[](
		const GlExtensionId extension_id) const noexcept = 0;
}; // GlExtensionManager

using GlExtensionManagerPtr = GlExtensionManager*;
using GlExtensionManagerUPtr = std::unique_ptr<GlExtensionManager>;


struct GlExtensionManagerFactory
{
	static GlExtensionManagerUPtr create();
}; // GlExtensionManagerFactory


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_EXTENSION_MANAGER_INCLUDED
