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
// OpenGL shader variable (implementation).
//
// !!! Internal usage only. !!!
//


#ifndef BSTONE_DETAIL_GL_SHADER_VAR_INCLUDED
#define BSTONE_DETAIL_GL_SHADER_VAR_INCLUDED


#include "bstone_renderer_3d.h"


namespace bstone
{
namespace detail
{


class GlShaderStage;
using GlShaderStagePtr = GlShaderStage*;


// ==========================================================================
// GlShaderVar
//

class GlShaderVar :
	public virtual Renderer3dShaderVarInt32,
	public virtual Renderer3dShaderVarFloat32,
	public virtual Renderer3dShaderVarVec2,
	public virtual Renderer3dShaderVarVec4,
	public virtual Renderer3dShaderVarMat4,
	public virtual Renderer3dShaderVarSampler2d
{
protected:
	GlShaderVar();


public:
	~GlShaderVar() override;


	static int get_unit_size(
		const Renderer3dShaderVarTypeId type_id);
}; // GlShaderVar

using GlShaderVarPtr = GlShaderVar*;
using GlShaderVarUPtr = std::unique_ptr<GlShaderVar>;

//
// GlShaderVar
// ==========================================================================


// ==========================================================================
// GlShaderVarFactory
//

struct GlShaderVarFactory
{
	struct CreateParam
	{
		Renderer3dShaderVarKind kind_;
		Renderer3dShaderVarTypeId type_id_;
		int value_size_;
		int index_;
		std::string name_;
		int input_index_;
		int gl_location_;
	}; // CreateParam

	static GlShaderVarUPtr create(
		const GlShaderStagePtr shader_stage,
		const CreateParam& param);
}; // GlShaderVarFactory

//
// GlShaderVarFactory
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_SHADER_VAR_INCLUDED
