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
// R3dMgrImpl
//

class R3dMgrImpl :
	public R3dMgr
{
public:
	R3dMgrImpl();

	R3dMgrImpl(
		R3dMgrImpl&& rhs);

	~R3dMgrImpl() override;


	R3dPtr renderer_initialize(
		const R3dCreateParam& param) override;


private:
	detail::R3dGlUPtr gl_renderer_;


	void initialize();

	void uninitialize();

	void uninitialize_renderers();
}; // R3dMgrImpl

//
// R3dMgrImpl
// ==========================================================================


// ==========================================================================
// R3dMgrImpl
//

R3dMgrImpl::R3dMgrImpl()
	:
	gl_renderer_{}
{
	initialize();
}

R3dMgrImpl::R3dMgrImpl(
	R3dMgrImpl&& rhs)
	:
	gl_renderer_{std::move(rhs.gl_renderer_)}
{
}

R3dMgrImpl::~R3dMgrImpl()
{
	uninitialize();
}

void R3dMgrImpl::initialize()
{
	detail::R3dGlUtils::load_library();
}

void R3dMgrImpl::uninitialize()
{
	uninitialize_renderers();

	detail::R3dGlUtils::unload_library();
}

void R3dMgrImpl::uninitialize_renderers()
{
	gl_renderer_ = nullptr;
}

R3dPtr R3dMgrImpl::renderer_initialize(
	const R3dCreateParam& param)
{
	uninitialize_renderers();

	detail::R3dUtils::validate_initialize_param(param);

	switch (param.renderer_kind_)
	{
#ifndef BSTONE_R3D_TEST_NO_GL

#ifndef BSTONE_R3D_TEST_NO_GL_2_0
		case R3dKind::gl_2_0:
#endif // !BSTONE_R3D_TEST_NO_GL_2_0

#ifndef BSTONE_R3D_TEST_NO_GL_3_2_C
		case R3dKind::gl_3_2_core:
#endif // !BSTONE_R3D_TEST_NO_GL_3_2_C

#ifndef BSTONE_R3D_TEST_NO_GLES_2_0
		case R3dKind::gles_2_0:
#endif // !BSTONE_R3D_TEST_NO_GLES_2_0

			gl_renderer_ = std::make_unique<detail::R3dGl>(param);

			return gl_renderer_.get();
#endif // BSTONE_R3D_TEST_NO_GL

		default:
			throw Exception{"Unsupported renderer kind."};
	}
}

//
// R3dMgrImpl
// ==========================================================================


// ==========================================================================
// R3dMgrFactory
//

R3dMgrUPtr R3dMgrFactory::create()
{
	return std::make_unique<R3dMgrImpl>();
}

//
// R3dMgrFactory
// ==========================================================================


} // bstone
