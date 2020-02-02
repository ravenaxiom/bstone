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
// OpenGL shader stage (implementation).
//
// !!! Internal usage only. !!!
//


#include "bstone_precompiled.h"
#include "bstone_detail_gl_shader_stage.h"

#include <unordered_set>

#include "bstone_exception.h"
#include "bstone_renderer_3d_tests.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_gl_shader_stage_manager.h"
#include "bstone_detail_gl_shader.h"
#include "bstone_detail_gl_renderer_3d_utils.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// GlShaderStage
//

GlShaderStage::GlShaderStage() = default;

GlShaderStage::~GlShaderStage() = default;

//
// GlShaderStage
// ==========================================================================


// ==========================================================================
// GlShaderStageImpl
//

class GlShaderStageImpl final :
	public GlShaderStage
{
public:
	GlShaderStageImpl(
		const GlShaderStageManagerPtr gl_shader_stage_manager,
		const Renderer3dShaderStageCreateParam& param);

	~GlShaderStageImpl() override;


	GlShaderStageManagerPtr get_manager() const noexcept override;


	void set() override;


	Renderer3dShaderVarPtr find_var(
		const std::string& name) override;

	Renderer3dShaderVarInt32Ptr find_var_int32(
		const std::string& name) override;

	Renderer3dShaderVarFloat32Ptr find_var_float32(
		const std::string& name) override;

	Renderer3dShaderVarVec2Ptr find_var_vec2(
		const std::string& name) override;

	Renderer3dShaderVarVec4Ptr find_var_vec4(
		const std::string& name) override;

	Renderer3dShaderVarMat4Ptr find_var_mat4(
		const std::string& name) override;

	Renderer3dShaderVarSampler2dPtr find_var_sampler_2d(
		const std::string& name) override;


	void detach_fragment_shader() override;

	void detach_vertex_shader() override;

	GLuint get_gl_name() const noexcept override;


private:
	using NameBuffer = std::vector<char>;

	const GlShaderStageManagerPtr gl_shader_stage_manager_;

	GlShaderPtr fragment_shader_;
	GlShaderPtr vertex_shader_;

	static void shader_stage_resource_deleter(
		const GLuint& gl_name) noexcept;

	using ShaderStageResource = UniqueResource<GLuint, shader_stage_resource_deleter>;

	ShaderStageResource gl_resource_;

	using ShaderVars = std::vector<GlShaderVarUPtr>;
	ShaderVars shader_vars_;


	void validate_shader(
		const Renderer3dShaderKind shader_kind,
		const Renderer3dShaderPtr shader);

	void validate_input_bindings(
		const Renderer3dShaderStageInputBindings& input_bindings);

	void validate_param(
		const Renderer3dShaderStageCreateParam& param);

	void set_input_bindings(
		const GLuint gl_name,
		const Renderer3dShaderStageInputBindings& input_bindings);

	int get_var_count(
		const GLuint gl_name);

	void get_vars(
		const Renderer3dShaderVarKind kind,
		const GLuint gl_name,
		ShaderVars& shader_vars);

	void check_input_bindings(
		const Renderer3dShaderStageInputBindings& input_bindings);

	void initialize(
		const Renderer3dShaderStageCreateParam& param);


	Renderer3dShaderVarPtr find_var_internal(
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
		const Renderer3dShaderVarTypeId type_id,
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
}; // GlShaderStageImpl

using GlShaderStageImplPtr = GlShaderStageImpl*;
using GlShaderStageImplUPtr = std::unique_ptr<GlShaderStageImpl>;

//
// GlShaderStageImpl
// ==========================================================================


// ==========================================================================
// GlShaderStageImpl
//

GlShaderStageImpl::GlShaderStageImpl(
	const GlShaderStageManagerPtr gl_shader_stage_manager,
	const Renderer3dShaderStageCreateParam& param)
	:
	gl_shader_stage_manager_{gl_shader_stage_manager},
	fragment_shader_{},
	vertex_shader_{},
	gl_resource_{},
	shader_vars_{}
{
	initialize(param);
}

GlShaderStageImpl::~GlShaderStageImpl()
{
	gl_shader_stage_manager_->notify_destroy(this);

	if (fragment_shader_)
	{
		fragment_shader_->attach_to_shader_stage(nullptr);
	}

	if (vertex_shader_)
	{
		vertex_shader_->attach_to_shader_stage(nullptr);
	}
}

GlShaderStageManagerPtr GlShaderStageImpl::get_manager() const noexcept
{
	return gl_shader_stage_manager_;
}

void GlShaderStageImpl::set()
{
	glUseProgram(gl_resource_);
	assert(!GlRenderer3dUtils::was_errors());
}

Renderer3dShaderVarPtr GlShaderStageImpl::find_var(
	const std::string& name)
{
	return find_var_internal(name);
}

Renderer3dShaderVarInt32Ptr GlShaderStageImpl::find_var_int32(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarInt32>(
		Renderer3dShaderVarTypeId::int32,
		name
	);
}

Renderer3dShaderVarFloat32Ptr GlShaderStageImpl::find_var_float32(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarFloat32>(
		Renderer3dShaderVarTypeId::float32,
		name
	);
}

Renderer3dShaderVarVec2Ptr GlShaderStageImpl::find_var_vec2(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarVec2>(
		Renderer3dShaderVarTypeId::vec2,
		name
	);
}

Renderer3dShaderVarVec4Ptr GlShaderStageImpl::find_var_vec4(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarVec4>(
		Renderer3dShaderVarTypeId::vec4,
		name
	);
}

Renderer3dShaderVarMat4Ptr GlShaderStageImpl::find_var_mat4(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarMat4>(
		Renderer3dShaderVarTypeId::mat4,
		name
	);
}

Renderer3dShaderVarSampler2dPtr GlShaderStageImpl::find_var_sampler_2d(
	const std::string& name)
{
	return find_var_internal<Renderer3dShaderVarSampler2d>(
		Renderer3dShaderVarTypeId::sampler2d,
		name
	);
}

void GlShaderStageImpl::initialize(
	const Renderer3dShaderStageCreateParam& param)
{
	if (!gl_shader_stage_manager_)
	{
		throw Exception{"Null shader stage manager."};
	}

	validate_param(param);

	gl_resource_.reset(glCreateProgram());

	if (!gl_resource_)
	{
		throw Exception{"Failed to create OpenGL program object."};
	}

	const auto fragment_shader = static_cast<GlShaderPtr>(param.fragment_shader_);
	glAttachShader(gl_resource_.get(), fragment_shader->get_gl_name());
	assert(!detail::GlRenderer3dUtils::was_errors());

	const auto vertex_shader = static_cast<GlShaderPtr>(param.vertex_shader_);
	glAttachShader(gl_resource_.get(), vertex_shader->get_gl_name());
	assert(!detail::GlRenderer3dUtils::was_errors());

	set_input_bindings(gl_resource_.get(), param.input_bindings_);

	glLinkProgram(gl_resource_.get());
	assert(!detail::GlRenderer3dUtils::was_errors());

	auto link_status = GLint{};

	glGetProgramiv(gl_resource_.get(), GL_LINK_STATUS, &link_status);
	assert(!detail::GlRenderer3dUtils::was_errors());

	if (link_status != GL_TRUE)
	{
		auto error_message = std::string{"Failed to link a program."};

		const auto gl_log = GlRenderer3dUtils::get_log(false, gl_resource_.get());

		if (!gl_log.empty())
		{
			error_message += '\n';
			error_message += gl_log;
		}

		throw Exception{error_message};
	}

	const auto var_count = get_var_count(gl_resource_.get());
	shader_vars_.reserve(var_count);

	get_vars(Renderer3dShaderVarKind::attribute, gl_resource_.get(), shader_vars_);

	// Note that "samplers" are included in uniforms.
	get_vars(Renderer3dShaderVarKind::uniform, gl_resource_.get(), shader_vars_);

	check_input_bindings(param.input_bindings_);

	fragment_shader_ = static_cast<GlShaderPtr>(param.fragment_shader_);
	vertex_shader_ = static_cast<GlShaderPtr>(param.vertex_shader_);
}

void GlShaderStageImpl::detach_fragment_shader()
{
	fragment_shader_ = nullptr;
}

void GlShaderStageImpl::detach_vertex_shader()
{
	vertex_shader_ = nullptr;
}

GLuint GlShaderStageImpl::get_gl_name() const noexcept
{
	return gl_resource_.get();
}

void GlShaderStageImpl::shader_stage_resource_deleter(
	const GLuint& gl_name) noexcept
{
	glDeleteProgram(gl_name);
	assert(!detail::GlRenderer3dUtils::was_errors());
}

void GlShaderStageImpl::validate_shader(
	const Renderer3dShaderKind shader_kind,
	const Renderer3dShaderPtr shader)
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

void GlShaderStageImpl::validate_input_bindings(
	const Renderer3dShaderStageInputBindings& input_bindings)
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

void GlShaderStageImpl::validate_param(
	const Renderer3dShaderStageCreateParam& param)
{
	validate_shader(Renderer3dShaderKind::fragment, param.fragment_shader_);
	validate_shader(Renderer3dShaderKind::vertex, param.vertex_shader_);
	validate_input_bindings(param.input_bindings_);
}

void GlShaderStageImpl::set_input_bindings(
	const GLuint gl_name,
	const Renderer3dShaderStageInputBindings& input_bindings)
{
	for (const auto& input_binding : input_bindings)
	{
		glBindAttribLocation(gl_name, input_binding.index_, input_binding.name_.c_str());
		assert(!detail::GlRenderer3dUtils::was_errors());
	}
}

int GlShaderStageImpl::get_var_count(
	const GLuint gl_name)
{
	auto gl_vertex_attribute_count = GLint{};
	glGetProgramiv(gl_name, GL_ACTIVE_ATTRIBUTES, &gl_vertex_attribute_count);
	assert(!detail::GlRenderer3dUtils::was_errors());

	auto gl_uniform_count = GLint{};
	glGetProgramiv(gl_name, GL_ACTIVE_UNIFORMS, &gl_uniform_count);
	assert(!detail::GlRenderer3dUtils::was_errors());

	const auto result = static_cast<int>(gl_vertex_attribute_count + gl_uniform_count);

	return result;
}

void GlShaderStageImpl::get_vars(
	const Renderer3dShaderVarKind kind,
	const GLuint gl_name,
	ShaderVars& shader_vars)
{
	using GlInfoFunction = void (APIENTRYP)(
		const GLuint program,
		const GLuint index,
		const GLsizei bufSize,
		GLsizei* const length,
		GLint* const size,
		GLenum* const type,
		GLchar* const name);

	bool is_attribute = false;
	bool is_uniform = false;
	auto gl_count_enum = GLenum{};
	auto gl_max_length_enum = GLenum{};
	auto gl_info_function = GlInfoFunction{};

	switch (kind)
	{
		case Renderer3dShaderVarKind::attribute:
			is_attribute = true;
			gl_count_enum = GL_ACTIVE_ATTRIBUTES;
			gl_max_length_enum = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH;
			gl_info_function = glGetActiveAttrib;
			break;

		case Renderer3dShaderVarKind::uniform:
			is_uniform = true;
			gl_count_enum = GL_ACTIVE_UNIFORMS;
			gl_max_length_enum = GL_ACTIVE_UNIFORM_MAX_LENGTH;
			gl_info_function = glGetActiveUniform;
			break;

		default:
			throw Exception{"Unsupported variable kind."};
	}

	auto gl_count = GLint{};
	glGetProgramiv(gl_name, gl_count_enum, &gl_count);
	assert(!detail::GlRenderer3dUtils::was_errors());

	if (gl_count <= 0)
	{
		return;
	}

	auto gl_max_length = GLint{};
	glGetProgramiv(gl_name, gl_max_length_enum, &gl_max_length);
	assert(!detail::GlRenderer3dUtils::was_errors());

	if (gl_max_length <= 0)
	{
		return;
	}

	auto name_buffer = NameBuffer{};
	name_buffer.resize(gl_max_length);

	for (int i = 0; i < gl_count; ++i)
	{
		auto gl_length = GLsizei{};
		auto gl_size = GLint{};
		auto gl_type = GLenum{};
		auto var_param = GlShaderVarFactory::CreateParam{};

		gl_info_function(
			gl_name,
			i,
			gl_max_length,
			&gl_length,
			&gl_size,
			&gl_type,
			name_buffer.data()
		);

		assert(!detail::GlRenderer3dUtils::was_errors());

		if (gl_length <= 0)
		{
			throw Exception{"Empty name."};
		}

		auto unit_count = 0;

		switch (gl_size)
		{
			case 1:
				unit_count = 1;
				break;

			default:
				throw Exception{"Unsupported unit count."};
		}

		bool is_sampler = false;
		auto unit_type_id = Renderer3dShaderVarTypeId{};

		switch (gl_type)
		{
			case GL_INT:
				unit_type_id = Renderer3dShaderVarTypeId::int32;
				break;

			case GL_FLOAT:
				unit_type_id = Renderer3dShaderVarTypeId::float32;
				break;

			case GL_FLOAT_VEC2:
				unit_type_id = Renderer3dShaderVarTypeId::vec2;
				break;

			case GL_FLOAT_VEC3:
				unit_type_id = Renderer3dShaderVarTypeId::vec3;
				break;

			case GL_FLOAT_VEC4:
				unit_type_id = Renderer3dShaderVarTypeId::vec4;
				break;

			case GL_FLOAT_MAT4:
				unit_type_id = Renderer3dShaderVarTypeId::mat4;
				break;

			case GL_SAMPLER_2D:
				is_sampler = true;
				unit_type_id = Renderer3dShaderVarTypeId::sampler2d;
				break;

			default:
				throw Exception{"Unsupported unit type."};
		}

		auto input_index = GLint{};

		if (is_attribute)
		{
			input_index = glGetAttribLocation(gl_name, name_buffer.data());
			assert(!detail::GlRenderer3dUtils::was_errors());

			if (input_index < 0)
			{
				throw Exception{"Vertex attribute not found."};
			}
		}
		else
		{
			input_index = -1;
		}

		const auto new_kind = (is_sampler ? Renderer3dShaderVarKind::sampler : kind);
		const auto index = static_cast<int>(shader_vars.size());
		const auto unit_size = GlShaderVar::get_unit_size(unit_type_id);
		const auto value_size = unit_count * unit_size;

		auto name = std::string{};
		name.assign(name_buffer.data(), static_cast<std::size_t>(gl_length));

		var_param.kind_ = new_kind;
		var_param.type_id_ = unit_type_id;
		var_param.value_size_ = value_size;
		var_param.index_ = index;
		var_param.name_ = std::move(name);
		var_param.input_index_ = input_index;
		var_param.gl_location_ = i;

		auto var = GlShaderVarFactory::create(this, var_param);

		shader_vars.emplace_back(std::move(var));
	}
}

void GlShaderStageImpl::check_input_bindings(
	const Renderer3dShaderStageInputBindings& input_bindings)
{
	for (const auto& input_binding : input_bindings)
	{
		const auto vertex_attribute = find_var_internal(input_binding.name_);

		if (!vertex_attribute)
		{
			throw Exception{"Vertex attribute not found."};
		}

		if (vertex_attribute->get_kind() != Renderer3dShaderVarKind::attribute)
		{
			throw Exception{"Not a vertex attribute."};
		}
	}
}

//
// GlShaderStageImpl
// ==========================================================================


// ==========================================================================
// GlShaderStageFactory
//

GlShaderStageUPtr GlShaderStageFactory::create(
	const GlShaderStageManagerPtr gl_shader_stage_manager,
	const Renderer3dShaderStageCreateParam& param)
{
	return std::make_unique<GlShaderStageImpl>(gl_shader_stage_manager, param);
}

//
// GlShaderStageFactory
// ==========================================================================


} // detail
} // bstone
