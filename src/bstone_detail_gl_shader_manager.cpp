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
#include "bstone_detail_gl_shader_manager.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"

#include "bstone_detail_gl_shader.h"
#include "bstone_detail_gl_context.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlShaderManagerImpl
//

class GlShaderManagerImpl final :
	public GlShaderManager
{
public:
	GlShaderManagerImpl(
		const GlContextPtr gl_context);

	~GlShaderManagerImpl() override;


	Renderer3dShaderUPtr create(
		const Renderer3dShaderCreateParam& param) override;

	void notify_destroy(
		const Renderer3dShaderPtr shader) noexcept override;


private:
	const GlContextPtr gl_context_;


	void initialize();
}; // GlShaderManagerImpl

using GlShaderManagerImplPtr = GlShaderManagerImpl*;
using GlShaderManagerImplUPtr = std::unique_ptr<GlShaderManagerImpl>;

//
// GlShaderManagerImpl
// ==========================================================================


// ==========================================================================
// GlShaderManagerImpl
//

GlShaderManagerImpl::GlShaderManagerImpl(
	const GlContextPtr gl_context)
	:
	gl_context_{gl_context}
{
	initialize();
}

GlShaderManagerImpl::~GlShaderManagerImpl() = default;

Renderer3dShaderUPtr GlShaderManagerImpl::create(
	const Renderer3dShaderCreateParam& param)
{
	return GlShaderFactory::create(this, param);
}

void GlShaderManagerImpl::notify_destroy(
	const Renderer3dShaderPtr shader) noexcept
{
	static_cast<void>(shader);
}

void GlShaderManagerImpl::initialize()
{
	if (!gl_context_)
	{
		throw Exception{"Null OpenGL context."};
	}
}

//
// GlShaderManagerImpl
// ==========================================================================


// ==========================================================================
// GlShaderManagerFactory
//

GlShaderManagerUPtr GlShaderManagerFactory::create(
	const GlContextPtr gl_context)
{
	return std::make_unique<GlShaderManagerImpl>(gl_context);
}

//
// GlShaderManagerFactory
// ==========================================================================


} // detail
} // bstone
