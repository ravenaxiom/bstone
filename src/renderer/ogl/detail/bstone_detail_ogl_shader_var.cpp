/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2019 Boris I. Bendovsky (bibendovsky@hotmail.com)

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
#include "bstone_detail_ogl_shader_var.h"

#include "glm/gtc/type_ptr.hpp"

#include "bstone_exception.h"
#include "bstone_ogl_api.h"

#include "bstone_detail_ogl_context.h"
#include "bstone_detail_ogl_renderer_utils.h"
#include "bstone_detail_ogl_shader_stage.h"
#include "bstone_detail_ogl_shader_stage_manager.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// OglShaderVar
//

OglShaderVar::OglShaderVar() = default;

OglShaderVar::~OglShaderVar() = default;

int OglShaderVar::get_unit_size(
	const RendererShaderVarTypeId type_id)
{
	switch (type_id)
	{
		case RendererShaderVarTypeId::int32:
		case RendererShaderVarTypeId::float32:
		case RendererShaderVarTypeId::sampler2d:
			return 4;

		case RendererShaderVarTypeId::vec2:
			return 2 * 4;

		case RendererShaderVarTypeId::vec3:
			return 3 * 4;

		case RendererShaderVarTypeId::vec4:
			return 4 * 4;

		case RendererShaderVarTypeId::mat4:
			return 4 * 4 * 4;

		default:
			throw Exception{"Unsupported type."};
	}
}

//
// OglShaderVar
// ==========================================================================


// ==========================================================================
// GenericOglShaderVar
//

class GenericOglShaderVar :
	public OglShaderVar
{
public:
	GenericOglShaderVar(
		const OglShaderStagePtr shader_stage,
		const OglShaderVarFactory::CreateParam& param);

	~GenericOglShaderVar() override;


	RendererShaderVarKind get_kind() const noexcept override;

	RendererShaderVarTypeId get_type_id() const noexcept override;

	int get_index() const noexcept override;

	const std::string& get_name() const noexcept override;

	int get_input_index() const noexcept override;


	void set_value(
		const std::int32_t value) override;

	void set_value(
		const float value) override;

	void set_value(
		const glm::vec2& value) override;

	void set_value(
		const glm::vec4& value) override;

	void set_value(
		const glm::mat4& value) override;


private:
	const OglShaderStagePtr shader_stage_;

	RendererShaderVarKind kind_;
	RendererShaderVarTypeId type_id_;
	int value_size_;
	int index_;
	std::string name_;
	int input_index_;
	int ogl_location_;


	void initialize(
		const OglShaderVarFactory::CreateParam& param);

	void set_value(
		const RendererShaderVarTypeId type_id,
		const void* const value_data);

	void set_value(
		const void* const value_data);
}; // GenericOglShaderVar

using GenericOglShaderVarPtr = GenericOglShaderVar*;
using GenericOglShaderVarUPtr = std::unique_ptr<GenericOglShaderVar>;

//
// GenericOglShaderVar
// ==========================================================================


// ==========================================================================
// GenericOglShaderVar
//

GenericOglShaderVar::GenericOglShaderVar(
	const OglShaderStagePtr shader_stage,
	const OglShaderVarFactory::CreateParam& param)
	:
	shader_stage_{shader_stage},
	kind_{},
	type_id_{},
	value_size_{},
	index_{},
	name_{},
	input_index_{},
	ogl_location_{}
{
	initialize(param);
}

GenericOglShaderVar::~GenericOglShaderVar() = default;

RendererShaderVarKind GenericOglShaderVar::get_kind() const noexcept
{
	return kind_;
}

RendererShaderVarTypeId GenericOglShaderVar::get_type_id() const noexcept
{
	return type_id_;
}

int GenericOglShaderVar::get_index() const noexcept
{
	return index_;
}

const std::string& GenericOglShaderVar::get_name() const noexcept
{
	return name_;
}

int GenericOglShaderVar::get_input_index() const noexcept
{
	return input_index_;
}

void GenericOglShaderVar::set_value(
	const std::int32_t value)
{
	set_value(RendererShaderVarTypeId::int32, &value);
}

void GenericOglShaderVar::set_value(
	const float value)
{
	set_value(RendererShaderVarTypeId::float32, &value);
}

void GenericOglShaderVar::set_value(
	const glm::vec2& value)
{
	set_value(RendererShaderVarTypeId::vec2, &value);
}

void GenericOglShaderVar::set_value(
	const glm::vec4& value)
{
	set_value(RendererShaderVarTypeId::vec4, &value);
}

void GenericOglShaderVar::set_value(
	const glm::mat4& value)
{
	set_value(RendererShaderVarTypeId::mat4, &value);
}

void GenericOglShaderVar::initialize(
	const OglShaderVarFactory::CreateParam& param)
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
	ogl_location_ = param.ogl_location_;
}

void GenericOglShaderVar::set_value(
	const RendererShaderVarTypeId type_id,
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
		case RendererShaderVarKind::sampler:
		case RendererShaderVarKind::uniform:
			break;

		default:
			throw Exception{"Changing vertex attribute not supported."};
	}


	const auto& ogl_device_features = shader_stage_->get_manager()->get_ogl_context()->get_ogl_device_features();

	if (!ogl_device_features.sso_is_available_)
	{
		shader_stage_->set();
	}

	set_value(value_data);
}

void GenericOglShaderVar::set_value(
	const void* const value_data)
{
	const auto& ogl_device_features = shader_stage_->get_manager()->get_ogl_context()->get_ogl_device_features();

	auto shader_stage_ogl_name = GLuint{};

	if (ogl_device_features.sso_is_available_)
	{
		shader_stage_ogl_name = shader_stage_->get_ogl_name();
	}

	switch (type_id_)
	{
		case RendererShaderVarTypeId::int32:
		case RendererShaderVarTypeId::sampler2d:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniform1iv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					static_cast<const GLint*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniform1iv(
					ogl_location_,
					1,
					static_cast<const GLint*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		case RendererShaderVarTypeId::float32:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniform1fv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniform1fv(
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		case RendererShaderVarTypeId::vec2:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniform2fv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniform2fv(
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		case RendererShaderVarTypeId::vec3:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniform3fv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniform3fv(
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		case RendererShaderVarTypeId::vec4:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniform4fv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniform4fv(
					ogl_location_,
					1,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		case RendererShaderVarTypeId::mat4:
			if (ogl_device_features.sso_is_available_)
			{
				::glProgramUniformMatrix4fv(
					shader_stage_ogl_name,
					ogl_location_,
					1,
					GL_FALSE,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}
			else
			{
				::glUniformMatrix4fv(
					ogl_location_,
					1,
					GL_FALSE,
					static_cast<const GLfloat*>(value_data)
				);

				assert(!OglRendererUtils::was_errors());
			}

			break;

		default:
			throw Exception{"Unsupported type."};
	}
}

//
// GenericOglShaderVar
// ==========================================================================


// ==========================================================================
// OglShaderVarFactory
//

OglShaderVarUPtr OglShaderVarFactory::create(
	const OglShaderStagePtr shader_stage,
	const OglShaderVarFactory::CreateParam& param)
{
	return std::make_unique<GenericOglShaderVar>(shader_stage, param);
}

//
// OglShaderVarFactory
// ==========================================================================


} // detail
} // bstone