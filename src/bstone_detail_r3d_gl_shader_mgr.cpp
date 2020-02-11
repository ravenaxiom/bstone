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
// OpenGL shader object manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_shader_mgr.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_shader.h"
#include "bstone_detail_r3d_gl_context.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlShaderMgrImpl
//

class R3dGlShaderMgrImpl final :
	public R3dGlShaderMgr
{
public:
	R3dGlShaderMgrImpl(
		const R3dGlContextPtr gl_context);

	~R3dGlShaderMgrImpl() override;


	R3dShaderUPtr create(
		const R3dShaderCreateParam& param) override;

	void notify_destroy(
		const R3dShaderPtr shader) noexcept override;


private:
	const R3dGlContextPtr gl_context_;


	void initialize();
}; // R3dGlShaderMgrImpl

using R3dGlShaderMgrImplPtr = R3dGlShaderMgrImpl*;
using R3dGlShaderMgrImplUPtr = std::unique_ptr<R3dGlShaderMgrImpl>;

//
// R3dGlShaderMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderMgrImpl
//

R3dGlShaderMgrImpl::R3dGlShaderMgrImpl(
	const R3dGlContextPtr gl_context)
	:
	gl_context_{gl_context}
{
	initialize();
}

R3dGlShaderMgrImpl::~R3dGlShaderMgrImpl() = default;

R3dShaderUPtr R3dGlShaderMgrImpl::create(
	const R3dShaderCreateParam& param)
{
	return R3dGlShaderFactory::create(this, param);
}

void R3dGlShaderMgrImpl::notify_destroy(
	const R3dShaderPtr shader) noexcept
{
	static_cast<void>(shader);
}

void R3dGlShaderMgrImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL context."};
	}
}

//
// R3dGlShaderMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderMgrFactory
//

R3dGlShaderMgrUPtr R3dGlShaderMgrFactory::create(
	const R3dGlContextPtr gl_context)
{
	return std::make_unique<R3dGlShaderMgrImpl>(gl_context);
}

//
// R3dGlShaderMgrFactory
// ==========================================================================


} // detail
} // bstone
