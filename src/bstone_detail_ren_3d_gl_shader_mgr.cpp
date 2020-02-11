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
#include "bstone_detail_ren_3d_gl_shader_mgr.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_shader.h"
#include "bstone_detail_ren_3d_gl_context.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGlShaderMgrImpl
//

class Ren3dGlShaderMgrImpl final :
	public Ren3dGlShaderMgr
{
public:
	Ren3dGlShaderMgrImpl(
		const Ren3dGlContextPtr gl_context);

	~Ren3dGlShaderMgrImpl() override;


	Ren3dShaderUPtr create(
		const Ren3dShaderCreateParam& param) override;

	void notify_destroy(
		const Ren3dShaderPtr shader) noexcept override;


private:
	const Ren3dGlContextPtr gl_context_;


	void initialize();
}; // Ren3dGlShaderMgrImpl

using Ren3dGlShaderMgrImplPtr = Ren3dGlShaderMgrImpl*;
using Ren3dGlShaderMgrImplUPtr = std::unique_ptr<Ren3dGlShaderMgrImpl>;

//
// Ren3dGlShaderMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlShaderMgrImpl
//

Ren3dGlShaderMgrImpl::Ren3dGlShaderMgrImpl(
	const Ren3dGlContextPtr gl_context)
	:
	gl_context_{gl_context}
{
	initialize();
}

Ren3dGlShaderMgrImpl::~Ren3dGlShaderMgrImpl() = default;

Ren3dShaderUPtr Ren3dGlShaderMgrImpl::create(
	const Ren3dShaderCreateParam& param)
{
	return Ren3dGlShaderFactory::create(this, param);
}

void Ren3dGlShaderMgrImpl::notify_destroy(
	const Ren3dShaderPtr shader) noexcept
{
	static_cast<void>(shader);
}

void Ren3dGlShaderMgrImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL context."};
	}
}

//
// Ren3dGlShaderMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlShaderMgrFactory
//

Ren3dGlShaderMgrUPtr Ren3dGlShaderMgrFactory::create(
	const Ren3dGlContextPtr gl_context)
{
	return std::make_unique<Ren3dGlShaderMgrImpl>(gl_context);
}

//
// Ren3dGlShaderMgrFactory
// ==========================================================================


} // detail
} // bstone
