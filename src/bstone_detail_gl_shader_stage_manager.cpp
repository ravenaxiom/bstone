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
// OpenGL shader stage manager (implementation).
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_shader_stage_manager.h"

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"

#include "bstone_detail_gl_context.h"
#include "bstone_detail_gl_shader_stage.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlShaderStageManager
//

GlShaderStageManager::GlShaderStageManager() = default;

GlShaderStageManager::~GlShaderStageManager() = default;

//
// GlShaderStageManager
// ==========================================================================


// ==========================================================================
// GlShaderStageManagerImpl
//

class GlShaderStageManagerImpl final :
	public GlShaderStageManager
{
public:
	GlShaderStageManagerImpl(
		const GlContextPtr gl_context);

	~GlShaderStageManagerImpl() override;


	GlContextPtr get_gl_context() const noexcept override;


	Renderer3dShaderStageUPtr create(
		const Renderer3dShaderStageCreateParam& param) override;

	void notify_destroy(
		const Renderer3dShaderStagePtr shader_stage) noexcept override;

	bool set_current(
		const Renderer3dShaderStagePtr shader_stage) override;


private:
	const GlContextPtr gl_context_;

	GlShaderStagePtr shader_stage_current_;


	void initialize();

	void shader_stage_set();
}; // GlShaderStageManagerImpl

using GlShaderStageManagerImplPtr = GlShaderStageManagerImpl*;
using GlShaderStageManagerImplUPtr = std::unique_ptr<GlShaderStageManagerImpl>;

//
// GlShaderStageManagerImpl
// ==========================================================================


// ==========================================================================
// GlShaderStageManagerImpl
//

GlShaderStageManagerImpl::GlShaderStageManagerImpl(
	const GlContextPtr gl_context)
	:
	gl_context_{gl_context},
	shader_stage_current_{}
{
	initialize();
}

GlShaderStageManagerImpl::~GlShaderStageManagerImpl() = default;

GlContextPtr GlShaderStageManagerImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

Renderer3dShaderStageUPtr GlShaderStageManagerImpl::create(
	const Renderer3dShaderStageCreateParam& param)
{
	return GlShaderStageFactory::create(this, param);
}

void GlShaderStageManagerImpl::notify_destroy(
	const Renderer3dShaderStagePtr shader_stage) noexcept
{
	if (shader_stage_current_ == shader_stage)
	{
		shader_stage_current_ = nullptr;
	}
}

bool GlShaderStageManagerImpl::set_current(
	const Renderer3dShaderStagePtr shader_stage)
{
	if (shader_stage_current_ == shader_stage)
	{
		return false;
	}

	shader_stage_current_ = static_cast<GlShaderStagePtr>(shader_stage);

	return true;
}

void GlShaderStageManagerImpl::initialize()
{
}

void GlShaderStageManagerImpl::shader_stage_set()
{
}

//
// GlShaderStageManagerImpl
// ==========================================================================


// ==========================================================================
// GlShaderStageManagerFactory
//

GlShaderStageManagerUPtr GlShaderStageManagerFactory::create(
	const GlContextPtr gl_context)
{
	return std::make_unique<GlShaderStageManagerImpl>(gl_context);
}

//
// GlShaderStageManagerFactory
// ==========================================================================


} // detail
} // bstone
