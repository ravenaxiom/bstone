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
// OpenGL shader variable (implementation).
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_r3d_gl_shader_var.h"

#include "bstone_exception.h"
#include "bstone_r3d_tests.h"

#include "bstone_detail_r3d_gl_api.h"
#include "bstone_detail_r3d_gl_context.h"
#include "bstone_detail_r3d_gl_error.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_shader_stage.h"
#include "bstone_detail_r3d_gl_shader_stage_mgr.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// R3dGlShaderVar
//

int R3dGlShaderVar::get_unit_size(
	const R3dShaderVarTypeId type_id)
{
	switch (type_id)
	{
		case R3dShaderVarTypeId::int32:
		case R3dShaderVarTypeId::float32:
		case R3dShaderVarTypeId::sampler2d:
			return 4;

		case R3dShaderVarTypeId::vec2:
			return 2 * 4;

		case R3dShaderVarTypeId::vec3:
			return 3 * 4;

		case R3dShaderVarTypeId::vec4:
			return 4 * 4;

		case R3dShaderVarTypeId::mat4:
			return 4 * 4 * 4;

		default:
			throw Exception{"Unsupported type."};
	}
}

//
// R3dGlShaderVar
// ==========================================================================


// ==========================================================================
// GlShaderVarImpl
//

class GlShaderVarImpl final :
	public R3dGlShaderVar
{
public:
	GlShaderVarImpl(
		const R3dGlShaderStagePtr shader_stage,
		const R3dGlShaderVarFactory::CreateParam& param);

	~GlShaderVarImpl() override;


	R3dShaderVarKind get_kind() const noexcept override;

	R3dShaderVarTypeId get_type_id() const noexcept override;

	int get_index() const noexcept override;

	const std::string& get_name() const noexcept override;

	int get_input_index() const noexcept override;


	void set_int(
		const std::int32_t value) override;

	void set_float(
		const float value) override;

	void set_vec2(
		const float* const value) override;

	void set_vec4(
		const float* const value) override;

	void set_mat4(
		const float* const value) override;

	void set_sampler_2d(
		const std::int32_t value) override;


private:
	const R3dGlShaderStagePtr shader_stage_;
	const R3dGlDeviceFeatures& gl_device_features_;

	R3dShaderVarKind kind_;
	R3dShaderVarTypeId type_id_;
	int value_size_;
	int index_;
	std::string name_;
	int input_index_;
	int gl_location_;


	void initialize(
		const R3dGlShaderVarFactory::CreateParam& param);

	void set_value(
		const R3dShaderVarTypeId type_id,
		const void* const value_data);

	void set_value(
		const void* const value_data);
}; // GlShaderVarImpl

using GlShaderVarImplPtr = GlShaderVarImpl*;
using GlShaderVarImplUPtr = std::unique_ptr<GlShaderVarImpl>;

//
// GlShaderVarImpl
// ==========================================================================


// ==========================================================================
// GlShaderVarImpl
//

GlShaderVarImpl::GlShaderVarImpl(
	const R3dGlShaderStagePtr shader_stage,
	const R3dGlShaderVarFactory::CreateParam& param)
	:
	shader_stage_{shader_stage},
	gl_device_features_{shader_stage->get_manager()->get_gl_context()->get_gl_device_features()},
	kind_{},
	type_id_{},
	value_size_{},
	index_{},
	name_{},
	input_index_{},
	gl_location_{}
{
	initialize(param);
}

GlShaderVarImpl::~GlShaderVarImpl() = default;

R3dShaderVarKind GlShaderVarImpl::get_kind() const noexcept
{
	return kind_;
}

R3dShaderVarTypeId GlShaderVarImpl::get_type_id() const noexcept
{
	return type_id_;
}

int GlShaderVarImpl::get_index() const noexcept
{
	return index_;
}

const std::string& GlShaderVarImpl::get_name() const noexcept
{
	return name_;
}

int GlShaderVarImpl::get_input_index() const noexcept
{
	return input_index_;
}

void GlShaderVarImpl::set_int(
	const std::int32_t value)
{
	set_value(R3dShaderVarTypeId::int32, &value);
}

void GlShaderVarImpl::set_float(
	const float value)
{
	set_value(R3dShaderVarTypeId::float32, &value);
}

void GlShaderVarImpl::set_vec2(
	const float* const value)
{
	set_value(R3dShaderVarTypeId::vec2, value);
}

void GlShaderVarImpl::set_vec4(
	const float* const value)
{
	set_value(R3dShaderVarTypeId::vec4, value);
}

void GlShaderVarImpl::set_mat4(
	const float* const value)
{
	set_value(R3dShaderVarTypeId::mat4, value);
}

void GlShaderVarImpl::set_sampler_2d(
	const std::int32_t value)
{
	set_value(R3dShaderVarTypeId::sampler2d, &value);
}

void GlShaderVarImpl::initialize(
	const R3dGlShaderVarFactory::CreateParam& param)
{
	if (!shader_stage_)
	{
		throw Exception{"Null shader stage."};
	}

	kind_ = param.kind_;
	type_id_ = param.type_id_;
	value_size_ = param.value_size_;
	index_ = param.index_;
	name_ = param.name_;
	input_index_ = param.input_index_;
	gl_location_ = param.gl_location_;
}

void GlShaderVarImpl::set_value(
	const R3dShaderVarTypeId type_id,
	const void* const value_data)
{
	if (type_id != type_id_)
	{
		throw Exception{"Mismatch type."};
	}

	if (!value_data)
	{
		throw Exception{"Null value data."};
	}

	const auto value_size = get_unit_size(type_id);

	if (value_size != value_size_)
	{
		throw Exception{"Value size mismatch."};
	}

	switch (kind_)
	{
		case R3dShaderVarKind::sampler:
		case R3dShaderVarKind::uniform:
			break;

		default:
			throw Exception{"Changing vertex attribute not supported."};
	}


	if (!gl_device_features_.sso_is_available_)
	{
		shader_stage_->set();
	}

	set_value(value_data);
}

void GlShaderVarImpl::set_value(
	const void* const value_data)
{
	auto shader_stage_gl_name = GLuint{};

	if (gl_device_features_.sso_is_available_)
	{
		shader_stage_gl_name = shader_stage_->get_gl_name();
	}

	switch (type_id_)
	{
		case R3dShaderVarTypeId::int32:
		case R3dShaderVarTypeId::sampler2d:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniform1iv(
					shader_stage_gl_name,
					gl_location_,
					1,
					static_cast<const GLint*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniform1iv(
					gl_location_,
					1,
					static_cast<const GLint*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		case R3dShaderVarTypeId::float32:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniform1fv(
					shader_stage_gl_name,
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniform1fv(
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		case R3dShaderVarTypeId::vec2:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniform2fv(
					shader_stage_gl_name,
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniform2fv(
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		case R3dShaderVarTypeId::vec3:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniform3fv(
					shader_stage_gl_name,
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniform3fv(
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		case R3dShaderVarTypeId::vec4:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniform4fv(
					shader_stage_gl_name,
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniform4fv(
					gl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		case R3dShaderVarTypeId::mat4:
			if (gl_device_features_.sso_is_available_)
			{
				glProgramUniformMatrix4fv(
					shader_stage_gl_name,
					gl_location_,
					1,
					GL_FALSE,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}
			else
			{
				glUniformMatrix4fv(
					gl_location_,
					1,
					GL_FALSE,
					static_cast<const GLfloat*>(value_data)
				);

				R3dGlError::ensure_debug();
			}

			break;

		default:
			throw Exception{"Unsupported type."};
	}
}

//
// GlShaderVarImpl
// ==========================================================================


// ==========================================================================
// R3dGlShaderVarFactory
//

R3dGlShaderVarUPtr R3dGlShaderVarFactory::create(
	const R3dGlShaderStagePtr shader_stage,
	const R3dGlShaderVarFactory::CreateParam& param)
{
	return std::make_unique<GlShaderVarImpl>(shader_stage, param);
}

//
// R3dGlShaderVarFactory
// ==========================================================================


} // detail
} // bstone
