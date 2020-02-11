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
#include "bstone_detail_ren_3d_gl_shader_stage_mgr.h"

#include "bstone_exception.h"
#include "bstone_ren_3d_tests.h"

#include "bstone_detail_ren_3d_gl_context.h"
#include "bstone_detail_ren_3d_gl_error.h"
#include "bstone_detail_ren_3d_gl_shader_stage.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dGlShaderStageMgrImpl
//

class Ren3dGlShaderStageMgrImpl final :
	public Ren3dGlShaderStageMgr
{
public:
	Ren3dGlShaderStageMgrImpl(
		const Ren3dGlContextPtr gl_context);

	~Ren3dGlShaderStageMgrImpl() override;


	Ren3dGlContextPtr get_gl_context() const noexcept override;


	Ren3dShaderStageUPtr create(
		const Ren3dShaderStageCreateParam& param) override;

	void notify_destroy(
		const Ren3dShaderStagePtr shader_stage) noexcept override;


	void set(
		const Ren3dShaderStagePtr shader_stage) override;


	Ren3dShaderStagePtr get_active() const noexcept override;

	void set_active(
		const Ren3dShaderStagePtr shader_stage) override;


	Ren3dShaderStagePtr get_current() const noexcept override;

	void set_current(
		const Ren3dShaderStagePtr shader_stage) override;


	void set_to_current() override;


private:
	const Ren3dGlContextPtr gl_context_;

	Ren3dShaderStagePtr shader_stage_active_;
	Ren3dShaderStagePtr shader_stage_current_;
}; // Ren3dGlShaderStageMgrImpl

using Ren3dGlShaderStageMgrImplPtr = Ren3dGlShaderStageMgrImpl*;
using Ren3dGlShaderStageMgrImplUPtr = std::unique_ptr<Ren3dGlShaderStageMgrImpl>;

//
// Ren3dGlShaderStageMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlShaderStageMgrImpl
//

Ren3dGlShaderStageMgrImpl::Ren3dGlShaderStageMgrImpl(
	const Ren3dGlContextPtr gl_context)
	:
	gl_context_{gl_context},
	shader_stage_active_{},
	shader_stage_current_{}
{
}

Ren3dGlShaderStageMgrImpl::~Ren3dGlShaderStageMgrImpl() = default;

Ren3dGlContextPtr Ren3dGlShaderStageMgrImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

Ren3dShaderStageUPtr Ren3dGlShaderStageMgrImpl::create(
	const Ren3dShaderStageCreateParam& param)
{
	return Ren3dGlShaderStageFactory::create(this, param);
}

void Ren3dGlShaderStageMgrImpl::notify_destroy(
	const Ren3dShaderStagePtr shader_stage) noexcept
{
	if (shader_stage == shader_stage_active_)
	{
		shader_stage_active_ = nullptr;
	}

	if (shader_stage_current_ == shader_stage)
	{
		shader_stage_current_ = nullptr;
	}
}

void Ren3dGlShaderStageMgrImpl::set(
	const Ren3dShaderStagePtr shader_stage)
{
	if (shader_stage == shader_stage_active_)
	{
		return;
	}

	if (shader_stage)
	{
		static_cast<Ren3dGlShaderStagePtr>(shader_stage)->set();
	}
	else
	{
		glUseProgram(0);
		Ren3dGlError::ensure_debug();

		shader_stage_active_ = nullptr;
	}
}

Ren3dShaderStagePtr Ren3dGlShaderStageMgrImpl::get_active() const noexcept
{
	return shader_stage_active_;
}

void Ren3dGlShaderStageMgrImpl::set_active(
	const Ren3dShaderStagePtr shader_stage)
{
	shader_stage_active_ = shader_stage;
}

Ren3dShaderStagePtr Ren3dGlShaderStageMgrImpl::get_current() const noexcept
{
	return shader_stage_current_;
}

void Ren3dGlShaderStageMgrImpl::set_current(
	const Ren3dShaderStagePtr shader_stage)
{
	shader_stage_current_ = shader_stage;
}

void Ren3dGlShaderStageMgrImpl::set_to_current()
{
	set(shader_stage_current_);
}

//
// Ren3dGlShaderStageMgrImpl
// ==========================================================================


// ==========================================================================
// Ren3dGlShaderStageMgrFactory
//

Ren3dGlShaderStageMgrUPtr Ren3dGlShaderStageMgrFactory::create(
	const Ren3dGlContextPtr gl_context)
{
	return std::make_unique<Ren3dGlShaderStageMgrImpl>(gl_context);
}

//
// Ren3dGlShaderStageMgrFactory
// ==========================================================================


} // detail
} // bstone
