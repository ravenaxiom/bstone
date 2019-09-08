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
// OpenGL shader stage (implementation).
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_ogl_shader_stage.h"

#include <unordered_set>

#include "bstone_exception.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_ogl_shader_stage_manager.h"
#include "bstone_detail_ogl_shader.h"
#include "bstone_detail_ogl_renderer_utils.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// OglShaderStage
//

OglShaderStage::OglShaderStage() = default;

OglShaderStage::~OglShaderStage() = default;

//
// OglShaderStage
// ==========================================================================


// ==========================================================================
// GenericOglShaderStage
//

class GenericOglShaderStage final :
	public OglShaderStage
{
public:
	GenericOglShaderStage(
		const OglShaderStageManagerPtr ogl_shader_stage_manager,
		const RendererShaderStageCreateParam& param);

	~GenericOglShaderStage() override;


	OglShaderStageManagerPtr get_manager() const noexcept override;


	void set() override;

	RendererShaderVarPtr find_var(
		const std::string& name) override;

	RendererShaderVarInt32Ptr find_var_int32(
		const std::string& name) override;

	RendererShaderVarFloat32Ptr find_var_float32(
		const std::string& name) override;

	RendererShaderVarVec2Ptr find_var_vec2(
		const std::string& name) override;

	RendererShaderVarVec4Ptr find_var_vec4(
		const std::string& name) override;

	RendererShaderVarMat4Ptr find_var_mat4(
		const std::string& name) override;

	RendererShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) override;


	void detach_fragment_shader() override;

	void detach_vertex_shader() override;

	GLuint get_ogl_name() const noexcept override;


private:
	using NameBuffer = std::vector<char>;

	const OglShaderStageManagerPtr ogl_shader_stage_manager_;

	OglShaderPtr fragment_shader_;
	OglShaderPtr vertex_shader_;

	static void shader_stage_resource_deleter(
		const GLuint& ogl_name) noexcept;

	using ShaderStageResource = UniqueResource<GLuint, shader_stage_resource_deleter>;

	ShaderStageResource ogl_resource_;

	using ShaderVars = std::vector<OglShaderVarUPtr>;
	ShaderVars shader_vars_;


	void validate_shader(
		const RendererShaderKind shader_kind,
		const RendererShaderPtr shader);

	void validate_input_bindings(
		const RendererShaderStageInputBindings& input_bindings);

	void validate_param(
		const RendererShaderStageCreateParam& param);

	void set_input_bindings(
		const GLuint ogl_name,
		const RendererShaderStageInputBindings& input_bindings);

	int get_var_count(
		const GLuint ogl_name);

	void get_vars(
		const RendererShaderVarKind kind,
		const GLuint ogl_name,
		ShaderVars& shader_vars);

	void check_input_bindings(
		const RendererShaderStageInputBindings& input_bindings);

	void initialize(
		const RendererShaderStageCreateParam& param);


	RendererShaderVarPtr find_var_internal(
		const std::string& name)
	{
		const auto end_it = shader_vars_.end();

		const auto it = std::find_if(
			shader_vars_.begin(),
			end_it,
			[&](const auto& item)
			{
				return name == item->get_name();
			}
		);

		if (it == end_it)
		{
			return nullptr;
		}

		return it->get();
	}

	template<typename T>
	T* find_var_internal(
		const RendererShaderVarTypeId type_id,
		const std::string& name)
	{
		const auto end_it = shader_vars_.end();

		const auto it = std::find_if(
			shader_vars_.begin(),
			end_it,
			[&](const auto& item)
			{
				return type_id == item->get_type_id() && name == item->get_name();
			}
		);

		if (it == end_it)
		{
			return nullptr;
		}

		return static_cast<T*>(it->get());
	}
}; // GenericOglShaderStage

using GenericOglShaderStagePtr = GenericOglShaderStage*;
using GenericOglShaderStageUPtr = std::unique_ptr<GenericOglShaderStage>;

//
// GenericOglShaderStage
// ==========================================================================


// ==========================================================================
// GenericOglShaderStage
//

GenericOglShaderStage::GenericOglShaderStage(
	const OglShaderStageManagerPtr ogl_shader_stage_manager,
	const RendererShaderStageCreateParam& param)
	:
	ogl_shader_stage_manager_{ogl_shader_stage_manager},
	fragment_shader_{},
	vertex_shader_{},
	ogl_resource_{},
	shader_vars_{}
{
	initialize(param);
}

GenericOglShaderStage::~GenericOglShaderStage()
{
	if (fragment_shader_ != nullptr)
	{
		fragment_shader_->attach_to_shader_stage(nullptr);
	}

	if (vertex_shader_ != nullptr)
	{
		vertex_shader_->attach_to_shader_stage(nullptr);
	}
}

OglShaderStageManagerPtr GenericOglShaderStage::get_manager() const noexcept
{
	return ogl_shader_stage_manager_;
}

void GenericOglShaderStage::set()
{
	if (!ogl_shader_stage_manager_->set_current(this))
	{
		return;
	}

	::glUseProgram(ogl_resource_);
	assert(!OglRendererUtils::was_errors());
}

RendererShaderVarPtr GenericOglShaderStage::find_var(
	const std::string& name)
{
	return find_var_internal(name);
}

RendererShaderVarInt32Ptr GenericOglShaderStage::find_var_int32(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarInt32>(
		RendererShaderVarTypeId::int32,
		name
	);
}

RendererShaderVarFloat32Ptr GenericOglShaderStage::find_var_float32(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarFloat32>(
		RendererShaderVarTypeId::float32,
		name
	);
}

RendererShaderVarVec2Ptr GenericOglShaderStage::find_var_vec2(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarVec2>(
		RendererShaderVarTypeId::vec2,
		name
	);
}

RendererShaderVarVec4Ptr GenericOglShaderStage::find_var_vec4(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarVec4>(
		RendererShaderVarTypeId::vec4,
		name
	);
}

RendererShaderVarMat4Ptr GenericOglShaderStage::find_var_mat4(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarMat4>(
		RendererShaderVarTypeId::mat4,
		name
	);
}

RendererShaderVarSampler2dPtr GenericOglShaderStage::find_var_sampler_2d(
	const std::string& name)
{
	return find_var_internal<RendererShaderVarSampler2d>(
		RendererShaderVarTypeId::sampler2d,
		name
	);
}

void GenericOglShaderStage::initialize(
	const RendererShaderStageCreateParam& param)
{
	if (!ogl_shader_stage_manager_)
	{
		throw Exception{"Null shader stage manager."};
	}

	validate_param(param);

	ogl_resource_.reset(::glCreateProgram());

	if (!ogl_resource_)
	{
		throw Exception{"Failed to create OpenGL program object."};
	}

	const auto fragment_shader = static_cast<OglShaderPtr>(param.fragment_shader_);
	::glAttachShader(ogl_resource_.get(), fragment_shader->get_ogl_name());
	assert(!detail::OglRendererUtils::was_errors());

	const auto vertex_shader = static_cast<OglShaderPtr>(param.vertex_shader_);
	::glAttachShader(ogl_resource_.get(), vertex_shader->get_ogl_name());
	assert(!detail::OglRendererUtils::was_errors());

	set_input_bindings(ogl_resource_.get(), param.input_bindings_);

	::glLinkProgram(ogl_resource_.get());
	assert(!detail::OglRendererUtils::was_errors());

	auto link_status = GLint{};

	::glGetProgramiv(ogl_resource_.get(), GL_LINK_STATUS, &link_status);
	assert(!detail::OglRendererUtils::was_errors());

	if (link_status != GL_TRUE)
	{
		auto error_message = std::string{"Failed to link a program."};

		const auto ogl_log = OglRendererUtils::get_log(false, ogl_resource_.get());

		if (!ogl_log.empty())
		{
			error_message += '\n';
			error_message += ogl_log;
		}

		throw Exception{std::move(error_message)};
	}

	const auto var_count = get_var_count(ogl_resource_.get());
	shader_vars_.reserve(var_count);

	get_vars(RendererShaderVarKind::attribute, ogl_resource_.get(), shader_vars_);

	// Note that "samplers" are included in uniforms.
	get_vars(RendererShaderVarKind::uniform, ogl_resource_.get(), shader_vars_);

	check_input_bindings(param.input_bindings_);

	fragment_shader_ = static_cast<OglShaderPtr>(param.fragment_shader_);
	vertex_shader_ = static_cast<OglShaderPtr>(param.vertex_shader_);
}

void GenericOglShaderStage::detach_fragment_shader()
{
	fragment_shader_ = nullptr;
}

void GenericOglShaderStage::detach_vertex_shader()
{
	vertex_shader_ = nullptr;
}

GLuint GenericOglShaderStage::get_ogl_name() const noexcept
{
	return ogl_resource_.get();
}

void GenericOglShaderStage::shader_stage_resource_deleter(
	const GLuint& ogl_name) noexcept
{
	::glDeleteProgram(ogl_name);
	assert(!detail::OglRendererUtils::was_errors());
}

void GenericOglShaderStage::validate_shader(
	const RendererShaderKind shader_kind,
	const RendererShaderPtr shader)
{
	if (shader == nullptr)
	{
		throw Exception{"Null shader."};
	}

	if (shader->get_kind() != shader_kind)
	{
		throw Exception{"Shader kind mismatch."};
	}
}

void GenericOglShaderStage::validate_input_bindings(
	const RendererShaderStageInputBindings& input_bindings)
{
	if (input_bindings.empty())
	{
		throw Exception{"No input bindings."};
	}

	// Check for duplicate names.
	//
	{
		using NameSetItem = const std::string*;

		struct NameSetItemComparer
		{
			bool operator()(
				const NameSetItem lhs,
				const NameSetItem rhs) const
			{
				return *lhs == *rhs;
			}
		}; // NameSetItemComparer

		using NameSet = std::unordered_set<NameSetItem, std::hash<NameSetItem>, NameSetItemComparer>;
		auto name_set = NameSet{};
		name_set.reserve(input_bindings.size());

		for (const auto& input_binding : input_bindings)
		{
			name_set.emplace(&input_binding.name_);
		}

		if (name_set.size() != input_bindings.size())
		{
			throw Exception{"Duplicate name."};
		}
	}

	// Check for duplicate indices.
	//
	{
		using NameSetItem = int;
		using NameSet = std::unordered_set<NameSetItem>;
		auto name_set = NameSet{};
		name_set.reserve(input_bindings.size());

		for (const auto& input_binding : input_bindings)
		{
			name_set.emplace(input_binding.index_);
		}

		if (name_set.size() != input_bindings.size())
		{
			throw Exception{"Duplicate index."};
		}
	}

	// Check for index value.
	//
	{
		for (const auto& input_binding : input_bindings)
		{
			if (input_binding.index_ < 0)
			{
				throw Exception{"Negative index."};
			}
		}
	}
}

void GenericOglShaderStage::validate_param(
	const RendererShaderStageCreateParam& param)
{
	validate_shader(RendererShaderKind::fragment, param.fragment_shader_);
	validate_shader(RendererShaderKind::vertex, param.vertex_shader_);
	validate_input_bindings(param.input_bindings_);
}

void GenericOglShaderStage::set_input_bindings(
	const GLuint ogl_name,
	const RendererShaderStageInputBindings& input_bindings)
{
	for (const auto& input_binding : input_bindings)
	{
		::glBindAttribLocation(ogl_name, input_binding.index_, input_binding.name_.c_str());
		assert(!detail::OglRendererUtils::was_errors());
	}
}

int GenericOglShaderStage::get_var_count(
	const GLuint ogl_name)
{
	auto ogl_vertex_attribute_count = GLint{};
	::glGetProgramiv(ogl_name, GL_ACTIVE_ATTRIBUTES, &ogl_vertex_attribute_count);
	assert(!detail::OglRendererUtils::was_errors());

	auto ogl_uniform_count = GLint{};
	::glGetProgramiv(ogl_name, GL_ACTIVE_UNIFORMS, &ogl_uniform_count);
	assert(!detail::OglRendererUtils::was_errors());

	const auto result = static_cast<int>(ogl_vertex_attribute_count + ogl_uniform_count);

	return result;
}

void GenericOglShaderStage::get_vars(
	const RendererShaderVarKind kind,
	const GLuint ogl_name,
	ShaderVars& shader_vars)
{
	using OglInfoFunction = void (APIENTRYP)(
		const GLuint program,
		const GLuint index,
		const GLsizei bufSize,
		GLsizei* const length,
		GLint* const size,
		GLenum* const type,
		GLchar* const name);

	bool is_attribute = false;
	bool is_uniform = false;
	auto ogl_count_enum = GLenum{};
	auto ogl_max_length_enum = GLenum{};
	auto ogl_info_function = OglInfoFunction{};

	switch (kind)
	{
		case RendererShaderVarKind::attribute:
			is_attribute = true;
			ogl_count_enum = GL_ACTIVE_ATTRIBUTES;
			ogl_max_length_enum = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH;
			ogl_info_function = ::glGetActiveAttrib;
			break;

		case RendererShaderVarKind::uniform:
			is_uniform = true;
			ogl_count_enum = GL_ACTIVE_UNIFORMS;
			ogl_max_length_enum = GL_ACTIVE_UNIFORM_MAX_LENGTH;
			ogl_info_function = ::glGetActiveUniform;
			break;

		default:
			throw Exception{"Unsupported variable kind."};
	}

	auto ogl_count = GLint{};
	::glGetProgramiv(ogl_name, ogl_count_enum, &ogl_count);
	assert(!detail::OglRendererUtils::was_errors());

	if (ogl_count <= 0)
	{
		return;
	}

	auto ogl_max_length = GLint{};
	::glGetProgramiv(ogl_name, ogl_max_length_enum, &ogl_max_length);
	assert(!detail::OglRendererUtils::was_errors());

	if (ogl_max_length <= 0)
	{
		return;
	}

	auto name_buffer = NameBuffer{};
	name_buffer.resize(ogl_max_length);

	for (int i = 0; i < ogl_count; ++i)
	{
		auto ogl_length = GLsizei{};
		auto ogl_size = GLint{};
		auto ogl_type = GLenum{};
		auto var_param = OglShaderVarFactory::CreateParam{};

		ogl_info_function(
			ogl_name,
			i,
			ogl_max_length,
			&ogl_length,
			&ogl_size,
			&ogl_type,
			name_buffer.data()
		);

		assert(!detail::OglRendererUtils::was_errors());

		if (ogl_length <= 0)
		{
			throw Exception{"Empty name."};
		}

		auto unit_count = 0;

		switch (ogl_size)
		{
			case 1:
				unit_count = 1;
				break;

			default:
				throw Exception{"Unsupported unit count."};
		}

		bool is_sampler = false;
		auto unit_type_id = RendererShaderVarTypeId{};

		switch (ogl_type)
		{
			case GL_INT:
				unit_type_id = RendererShaderVarTypeId::int32;
				break;

			case GL_FLOAT:
				unit_type_id = RendererShaderVarTypeId::float32;
				break;

			case GL_FLOAT_VEC2:
				unit_type_id = RendererShaderVarTypeId::vec2;
				break;

			case GL_FLOAT_VEC3:
				unit_type_id = RendererShaderVarTypeId::vec3;
				break;

			case GL_FLOAT_VEC4:
				unit_type_id = RendererShaderVarTypeId::vec4;
				break;

			case GL_FLOAT_MAT4:
				unit_type_id = RendererShaderVarTypeId::mat4;
				break;

			case GL_SAMPLER_2D:
				is_sampler = true;
				unit_type_id = RendererShaderVarTypeId::sampler2d;
				break;

			default:
				throw Exception{"Unsupported unit type."};
		}

		auto input_index = GLint{};

		if (is_attribute)
		{
			input_index = ::glGetAttribLocation(ogl_name, name_buffer.data());
			assert(!detail::OglRendererUtils::was_errors());

			if (input_index < 0)
			{
				throw Exception{"Vertex attribute not found."};
			}
		}
		else
		{
			input_index = -1;
		}

		const auto new_kind = (is_sampler ? RendererShaderVarKind::sampler : kind);
		const auto index = static_cast<int>(shader_vars.size());
		const auto unit_size = OglShaderVar::get_unit_size(unit_type_id);
		const auto value_size = unit_count * unit_size;

		auto name = std::string{};
		name.assign(name_buffer.data(), static_cast<std::size_t>(ogl_length));

		var_param.kind_ = new_kind;
		var_param.type_id_ = unit_type_id;
		var_param.value_size_ = value_size;
		var_param.index_ = index;
		var_param.name_ = std::move(name);
		var_param.input_index_ = input_index;
		var_param.ogl_location_ = i;

		auto var = OglShaderVarFactory::create(this, var_param);

		shader_vars.emplace_back(std::move(var));
	}
}

void GenericOglShaderStage::check_input_bindings(
	const RendererShaderStageInputBindings& input_bindings)
{
	for (const auto& input_binding : input_bindings)
	{
		const auto vertex_attribute = find_var_internal(input_binding.name_);

		if (!vertex_attribute)
		{
			throw Exception{"Vertex attribute not found."};
		}

		if (vertex_attribute->get_kind() != RendererShaderVarKind::attribute)
		{
			throw Exception{"Not a vertex attribute."};
		}
	}
}

//
// GenericOglShaderStage
// ==========================================================================


// ==========================================================================
// OglShaderStageFactory
//

OglShaderStageUPtr OglShaderStageFactory::create(
	const OglShaderStageManagerPtr ogl_shader_stage_manager,
	const RendererShaderStageCreateParam& param)
{
	return std::make_unique<GenericOglShaderStage>(ogl_shader_stage_manager, param);
}

//
// OglShaderStageFactory
// ==========================================================================


} // detail
} // bstone