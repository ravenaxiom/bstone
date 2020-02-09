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
// 3D renderer manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_r3d_mgr.h"

#include <vector>

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl.h"
#include "bstone_detail_r3d_gl_utils.h"


namespace bstone
{


// ==========================================================================
// Renderer3dManagerImpl
//

class Renderer3dManagerImpl :
	public Renderer3dManager
{
public:
	Renderer3dManagerImpl();

	Renderer3dManagerImpl(
		Renderer3dManagerImpl&& rhs);

	~Renderer3dManagerImpl() override;


	Renderer3dPtr renderer_initialize(
		const Renderer3dCreateParam& param) override;


private:
	detail::GlRenderer3dUPtr gl_renderer_;


	void initialize();

	void uninitialize();

	void uninitialize_renderers();
}; // Renderer3dManagerImpl

//
// Renderer3dManagerImpl
// ==========================================================================


// ==========================================================================
// Renderer3dManagerImpl
//

Renderer3dManagerImpl::Renderer3dManagerImpl()
	:
	gl_renderer_{}
{
	initialize();
}

Renderer3dManagerImpl::Renderer3dManagerImpl(
	Renderer3dManagerImpl&& rhs)
	:
	gl_renderer_{std::move(rhs.gl_renderer_)}
{
}

Renderer3dManagerImpl::~Renderer3dManagerImpl()
{
	uninitialize();
}

void Renderer3dManagerImpl::initialize()
{
	detail::GlRenderer3dUtils::load_library();
}

void Renderer3dManagerImpl::uninitialize()
{
	uninitialize_renderers();

	detail::GlRenderer3dUtils::unload_library();
}

void Renderer3dManagerImpl::uninitialize_renderers()
{
	gl_renderer_ = nullptr;
}

Renderer3dPtr Renderer3dManagerImpl::renderer_initialize(
	const Renderer3dCreateParam& param)
{
	uninitialize_renderers();

	detail::Renderer3dUtils::validate_initialize_param(param);

	switch (param.renderer_kind_)
	{
#ifndef BSTONE_RENDERER_3D_TEST_NO_GL

#ifndef BSTONE_RENDERER_3D_TEST_NO_GL_2_0
		case Renderer3dKind::gl_2_0:
#endif // !BSTONE_RENDERER_3D_TEST_NO_GL_2_0

#ifndef BSTONE_RENDERER_3D_TEST_NO_GL_3_2_C
		case Renderer3dKind::gl_3_2_core:
#endif // !BSTONE_RENDERER_3D_TEST_NO_GL_3_2_C

#ifndef BSTONE_RENDERER_3D_TEST_NO_GLES_2_0
		case Renderer3dKind::gles_2_0:
#endif // !BSTONE_RENDERER_3D_TEST_NO_GLES_2_0

			gl_renderer_ = std::make_unique<detail::GlRenderer3d>(param);

			return gl_renderer_.get();
#endif // BSTONE_RENDERER_3D_TEST_NO_GL

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

//
// Renderer3dManagerImpl
// ==========================================================================


// ==========================================================================
// Renderer3dManagerFactory
//

Renderer3dManagerUPtr Renderer3dManagerFactory::create()
{
	return std::make_unique<Renderer3dManagerImpl>();
}

//
// Renderer3dManagerFactory
// ==========================================================================


} // bstone
