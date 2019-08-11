/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

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
// OpenGL handle wrappers.
//


#ifndef BSTONE_OGL_HANDLES_INCLUDED
#define BSTONE_OGL_HANDLES_INCLUDED


#include "bstone_ogl.h"
#include "bstone_system_handle.h"


namespace bstone
{


void ogl_buffer_deleter(
	const GLuint ogl_name) noexcept;

using OglBufferHandle = SystemHandle<GLuint, 0, ogl_buffer_deleter>;


void ogl_shader_deleter(
	const GLuint ogl_name) noexcept;

using OglShaderHandle = SystemHandle<GLuint, 0, ogl_shader_deleter>;


void ogl_program_deleter(
	const GLuint ogl_name) noexcept;

using OglProgramHandle = SystemHandle<GLuint, 0, ogl_program_deleter>;


} // bstone


#endif // !BSTONE_OGL_HANDLES_INCLUDED