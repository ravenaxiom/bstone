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
// OpenGL shader stage (implementation).
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_DETAIL_GL_SHADER_STAGE_INCLUDED
#define BSTONE_DETAIL_GL_SHADER_STAGE_INCLUDED


#include "bstone_r3d_shader_stage.h"

#include "bstone_detail_r3d_gl_api.h"


namespace bstone
{
namespace detail
{


class GlShaderStageManager;
using GlShaderStageManagerPtr = GlShaderStageManager*;


// ==========================================================================
// GlShaderStage
//

class GlShaderStage :
	public Renderer3dShaderStage
{
protected:
	GlShaderStage() = default;


public:
	~GlShaderStage() override = default;


	virtual GlShaderStageManagerPtr get_manager() const noexcept = 0;


	virtual void set() = 0;

	virtual void detach_fragment_shader() = 0;

	virtual void detach_vertex_shader() = 0;

	virtual GLuint get_gl_name() const noexcept = 0;
}; // GlShaderStage

using GlShaderStagePtr = GlShaderStage*;
using GlShaderStageUPtr = std::unique_ptr<GlShaderStage>;

//
// GlShaderStage
// ==========================================================================


// ==========================================================================
// GlShaderStageFactory
//

struct GlShaderStageFactory
{
	static GlShaderStageUPtr create(
		const GlShaderStageManagerPtr gl_shader_stage_manager,
		const Renderer3dShaderStageCreateParam& param);
}; // GlShaderStageFactory

//
// GlShaderStageFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_SHADER_STAGE_INCLUDED
