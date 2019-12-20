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
// 3D renderer (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_renderer_3d.h"
#include "bstone_detail_renderer_3d_command_manager.h"


namespace bstone
{


// ==========================================================================
// Renderer3dBuffer
//

Renderer3dBuffer::Renderer3dBuffer() = default;

Renderer3dBuffer::~Renderer3dBuffer() = default;

//
// Renderer3dBuffer
// ==========================================================================


// ==========================================================================
// Shader
//

Renderer3dShaderVar::Renderer3dShaderVar() = default;

Renderer3dShaderVar::~Renderer3dShaderVar() = default;


Renderer3dShaderVarInt32::Renderer3dShaderVarInt32() = default;

Renderer3dShaderVarInt32::~Renderer3dShaderVarInt32() = default;


Renderer3dShaderVarFloat32::Renderer3dShaderVarFloat32() = default;

Renderer3dShaderVarFloat32::~Renderer3dShaderVarFloat32() = default;


Renderer3dShaderVarVec2::Renderer3dShaderVarVec2() = default;

Renderer3dShaderVarVec2::~Renderer3dShaderVarVec2() = default;


Renderer3dShaderVarVec4::Renderer3dShaderVarVec4() = default;

Renderer3dShaderVarVec4::~Renderer3dShaderVarVec4() = default;


Renderer3dShaderVarMat4::Renderer3dShaderVarMat4() = default;

Renderer3dShaderVarMat4::~Renderer3dShaderVarMat4() = default;

Renderer3dShaderVarSampler2d::Renderer3dShaderVarSampler2d() = default;

Renderer3dShaderVarSampler2d::~Renderer3dShaderVarSampler2d() = default;


Renderer3dShader::Renderer3dShader() = default;

Renderer3dShader::~Renderer3dShader() = default;


Renderer3dShaderStage::Renderer3dShaderStage() = default;

Renderer3dShaderStage::~Renderer3dShaderStage() = default;

//
// Shader
// ==========================================================================


// ==========================================================================
// Renderer3dCommandManagerFactory
//

Renderer3dCommandManagerUPtr Renderer3dCommandManagerFactory::create()
{
	return detail::RendererCommandManagerImplUPtr{new detail::Renderer3dCommandManagerImpl{}};
}

//
// Renderer3dCommandManagerFactory
// ==========================================================================


} // bstone
