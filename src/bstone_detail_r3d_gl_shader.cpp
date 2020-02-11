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
// OpenGL shader (implementation).
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_shader.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_shader_mgr.h"
#include "bstone_detail_r3d_gl_shader_stage.h"
#include "bstone_detail_r3d_gl_utils.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlShaderImpl
//

class GlShaderImpl final :
	public R3dGlShader
{
public:
	GlShaderImpl(
		const R3dGlShaderMgrPtr gl_shader_manager,
		const R3dShaderCreateParam& param);

	~GlShaderImpl() override;


	R3dShaderKind get_kind() const noexcept override;

	GLuint get_gl_name() const noexcept override;

	void attach_to_shader_stage(
		const R3dGlShaderStagePtr shader_stage) override;


private:
	const R3dGlShaderMgrPtr gl_shader_manager_;

	R3dShaderKind kind_;

	static void shader_resource_deleter(
		const GLuint& gl_name) noexcept;

	using ShaderResource = UniqueResource<GLuint, shader_resource_deleter>;

	ShaderResource gl_resource_;

	R3dGlShaderStagePtr shader_stage_;


	void initialize(
		const R3dShaderCreateParam& param);

	GLenum get_gl_kind(
		const R3dShaderKind kind);

	void validate_param(
		const R3dShaderCreateParam& param);
}; // GlShaderImpl

using GlShaderImplPtr = GlShaderImpl*;
using GlShaderImplUPtr = std::unique_ptr<GlShaderImpl>;

//
// GlShaderImpl
// ==========================================================================


// ==========================================================================
// GlShaderImpl
//

GlShaderImpl::GlShaderImpl(
	const R3dGlShaderMgrPtr gl_shader_manager,
	const R3dShaderCreateParam& param)
	:
	gl_shader_manager_{gl_shader_manager},
	kind_{},
	gl_resource_{},
	shader_stage_{}
{
	initialize(param);
}

GlShaderImpl::~GlShaderImpl()
{
	gl_shader_manager_->notify_destroy(this);

	if (shader_stage_)
	{
		switch (kind_)
		{
			case R3dShaderKind::fragment:
				shader_stage_->detach_fragment_shader();
				break;

			case R3dShaderKind::vertex:
				shader_stage_->detach_vertex_shader();
				break;

			default:
				assert(!"Unsupported kind.");
				break;
		}
	}
}

R3dShaderKind GlShaderImpl::get_kind() const noexcept
{
	return kind_;
}

void GlShaderImpl::shader_resource_deleter(
	const GLuint& gl_name) noexcept
{
	glDeleteShader(gl_name);
	R3dGlError::ensure_debug();
}

void GlShaderImpl::initialize(
	const R3dShaderCreateParam& param)
{
	validate_param(param);

	const auto gl_kind = get_gl_kind(param.kind_);

	gl_resource_.reset(glCreateShader(gl_kind));

	if (!gl_resource_)
	{
		throw Exception{"Failed to create OpenGL shader object."};
	}

	const char* const strings[] =
	{
		static_cast<const char*>(param.source_.data_),
	};

	const GLint lengths[] =
	{
		param.source_.size_,
	};

	glShaderSource(gl_resource_.get(), 1, strings, lengths);
	R3dGlError::ensure_debug();

	glCompileShader(gl_resource_.get());
	R3dGlError::ensure_debug();

	auto compile_status = GLint{};

	glGetShaderiv(gl_resource_.get(), GL_COMPILE_STATUS, &compile_status);
	R3dGlError::ensure_debug();

	if (compile_status != GL_TRUE)
	{
		auto error_message = std::string{"Failed to compile a shader."};

		const auto gl_log = R3dGlUtils::get_log(true, gl_resource_.get());

		if (!gl_log.empty())
		{
			error_message += '\n';
			error_message += gl_log;
		}

		throw Exception{error_message};
	}

	kind_ = param.kind_;
}

GLuint GlShaderImpl::get_gl_name() const noexcept
{
	return gl_resource_.get();
}

void GlShaderImpl::attach_to_shader_stage(
	const R3dGlShaderStagePtr shader_stage)
{
	shader_stage_ = shader_stage;
}

GLenum GlShaderImpl::get_gl_kind(
	const R3dShaderKind kind)
{
	switch (kind)
	{
		case R3dShaderKind::fragment:
			return GL_FRAGMENT_SHADER;

		case R3dShaderKind::vertex:
			return GL_VERTEX_SHADER;

		default:
			throw Exception{"Invalid kind."};
	}
}

void GlShaderImpl::validate_param(
	const R3dShaderCreateParam& param)
{
	switch (param.kind_)
	{
		case R3dShaderKind::fragment:
		case R3dShaderKind::vertex:
			break;

		default:
			throw Exception{"Invalid kind."};
	}

	if (param.source_.data_ == nullptr)
	{
		throw Exception{"Null source data."};
	}

	if (param.source_.size_ <= 0)
	{
		throw Exception{"Empty source data."};
	}
}

//
// GlShaderImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderFactory
//

R3dGlShaderUPtr R3dGlShaderFactory::create(
	const R3dGlShaderMgrPtr gl_shader_manager,
	const R3dShaderCreateParam& param)
{
	return std::make_unique<GlShaderImpl>(gl_shader_manager, param);
}

//
// R3dGlShaderFactory
// ==========================================================================


} // detail
} // bstone
