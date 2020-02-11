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
#include "bstone_detail_r3d_gl_shader_stage_mgr.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_shader_stage.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlShaderStageMgrImpl
//

class R3dGlShaderStageMgrImpl final :
	public R3dGlShaderStageMgr
{
public:
	R3dGlShaderStageMgrImpl(
		const R3dGlContextPtr gl_context);

	~R3dGlShaderStageMgrImpl() override;


	R3dGlContextPtr get_gl_context() const noexcept override;


	R3dShaderStageUPtr create(
		const R3dShaderStageCreateParam& param) override;

	void notify_destroy(
		const R3dShaderStagePtr shader_stage) noexcept override;


	void set(
		const R3dShaderStagePtr shader_stage) override;


	R3dShaderStagePtr get_active() const noexcept override;

	void set_active(
		const R3dShaderStagePtr shader_stage) override;


	R3dShaderStagePtr get_current() const noexcept override;

	void set_current(
		const R3dShaderStagePtr shader_stage) override;


	void set_to_current() override;


private:
	const R3dGlContextPtr gl_context_;

	R3dShaderStagePtr shader_stage_active_;
	R3dShaderStagePtr shader_stage_current_;
}; // R3dGlShaderStageMgrImpl

using R3dGlShaderStageMgrImplPtr = R3dGlShaderStageMgrImpl*;
using R3dGlShaderStageMgrImplUPtr = std::unique_ptr<R3dGlShaderStageMgrImpl>;

//
// R3dGlShaderStageMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderStageMgrImpl
//

R3dGlShaderStageMgrImpl::R3dGlShaderStageMgrImpl(
	const R3dGlContextPtr gl_context)
	:
	gl_context_{gl_context},
	shader_stage_active_{},
	shader_stage_current_{}
{
}

R3dGlShaderStageMgrImpl::~R3dGlShaderStageMgrImpl() = default;

R3dGlContextPtr R3dGlShaderStageMgrImpl::get_gl_context() const noexcept
{
	return gl_context_;
}

R3dShaderStageUPtr R3dGlShaderStageMgrImpl::create(
	const R3dShaderStageCreateParam& param)
{
	return R3dGlShaderStageFactory::create(this, param);
}

void R3dGlShaderStageMgrImpl::notify_destroy(
	const R3dShaderStagePtr shader_stage) noexcept
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

void R3dGlShaderStageMgrImpl::set(
	const R3dShaderStagePtr shader_stage)
{
	if (shader_stage == shader_stage_active_)
	{
		return;
	}

	if (shader_stage)
	{
		static_cast<R3dGlShaderStagePtr>(shader_stage)->set();
	}
	else
	{
		glUseProgram(0);
		R3dGlError::ensure_debug();

		shader_stage_active_ = nullptr;
	}
}

R3dShaderStagePtr R3dGlShaderStageMgrImpl::get_active() const noexcept
{
	return shader_stage_active_;
}

void R3dGlShaderStageMgrImpl::set_active(
	const R3dShaderStagePtr shader_stage)
{
	shader_stage_active_ = shader_stage;
}

R3dShaderStagePtr R3dGlShaderStageMgrImpl::get_current() const noexcept
{
	return shader_stage_current_;
}

void R3dGlShaderStageMgrImpl::set_current(
	const R3dShaderStagePtr shader_stage)
{
	shader_stage_current_ = shader_stage;
}

void R3dGlShaderStageMgrImpl::set_to_current()
{
	set(shader_stage_current_);
}

//
// R3dGlShaderStageMgrImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderStageMgrFactory
//

R3dGlShaderStageMgrUPtr R3dGlShaderStageMgrFactory::create(
	const R3dGlContextPtr gl_context)
{
	return std::make_unique<R3dGlShaderStageMgrImpl>(gl_context);
}

//
// R3dGlShaderStageMgrFactory
// ==========================================================================


} // detail
} // bstone
