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
// OpenGL shader stage manager (implementation interface).
//


#ifndef BSTONE_DETAIL_GL_SHADER_STAGE_MANAGER_INCLUDED
#define BSTONE_DETAIL_GL_SHADER_STAGE_MANAGER_INCLUDED


#include <memory>

#include "bstone_renderer_3d_shader_stage.h"


namespace bstone
{
namespace detail
{


class GlContext;
using GlContextPtr = GlContext*;


// ==========================================================================
// GlShaderStageManager
//

class GlShaderStageManager
{
protected:
	GlShaderStageManager() = default;


public:
	virtual ~GlShaderStageManager() = default;


	virtual GlContextPtr get_gl_context() const noexcept = 0;


	virtual Renderer3dShaderStageUPtr create(
		const Renderer3dShaderStageCreateParam& param) = 0;


	virtual void notify_destroy(
		const Renderer3dShaderStagePtr shader_stage) noexcept = 0;


	virtual void set(
		const Renderer3dShaderStagePtr shader_stage) = 0;


	virtual Renderer3dShaderStagePtr get_active() const noexcept = 0;

	virtual void set_active(
		const Renderer3dShaderStagePtr shader_stage) = 0;


	virtual Renderer3dShaderStagePtr get_current() const noexcept = 0;

	virtual void set_current(
		const Renderer3dShaderStagePtr shader_stage) = 0;


	virtual void synchronize() = 0;
}; // GlShaderStageManager

using GlShaderStageManagerPtr = GlShaderStageManager*;
using GlShaderStageManagerUPtr = std::unique_ptr<GlShaderStageManager>;

//
// GlShaderStageManager
// ==========================================================================


// ==========================================================================
// GlShaderStageManagerFactory
//

struct GlShaderStageManagerFactory
{
	static GlShaderStageManagerUPtr create(
		const GlContextPtr gl_context);
}; // GlShaderStageManagerFactory

//
// GlShaderStageManager
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_SHADER_STAGE_MANAGER_INCLUDED
