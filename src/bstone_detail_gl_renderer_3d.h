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
// OpenGL 3D renderer (implementation).
//
// !!! Internal usage only !!!
//


#ifndef BSTONE_DETAIL_GL_RENDERER_3D_INCLUDED
#define BSTONE_DETAIL_GL_RENDERER_3D_INCLUDED


#include <array>
#include <list>
#include <vector>

#include "bstone_renderer_3d.h"
#include "bstone_renderer_3d_command_buffer.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_gl_buffer.h"
#include "bstone_detail_gl_extension_manager.h"
#include "bstone_detail_gl_shader.h"
#include "bstone_detail_gl_shader_stage.h"
#include "bstone_detail_gl_renderer_3d_utils.h"
#include "bstone_detail_gl_context.h"


namespace bstone
{
namespace detail
{


class GlRenderer3d final :
	public Renderer3d
{
public:
	GlRenderer3d(
		const Renderer3dCreateParam& param);

	~GlRenderer3d() override;


	Renderer3dKind get_kind() const noexcept override;

	const std::string& get_name() const noexcept override;

	const std::string& get_description() const noexcept override;


	const Renderer3dDeviceFeatures& device_get_features() const noexcept override;

	const Renderer3dDeviceInfo& device_get_info() const noexcept override;


	void window_set_mode(
		const Renderer3dWindowSetModeParam& param) override;

	void window_set_title(
		const std::string& title_utf8) override;

	void window_show(
		const bool is_visible) override;


	bool vsync_get() const noexcept override;

	void vsync_set(
		const bool is_enabled) override;


	void aa_set(
		const Renderer3dAaKind aa_kind,
		const int aa_value) override;


	void present() override;


	Renderer3dBufferUPtr buffer_create(
		const Renderer3dBufferCreateParam& param) override;

	Renderer3dTexture2dUPtr texture_2d_create(
		const Renderer3dTexture2dCreateParam& param) override;

	Renderer3dSamplerUPtr sampler_create(
		const Renderer3dSamplerCreateParam& param) override;

	Renderer3dVertexInputUPtr vertex_input_create(
		const Renderer3dVertexInputCreateParam& param) override;

	Renderer3dShaderUPtr shader_create(
		const Renderer3dShaderCreateParam& param) override;

	Renderer3dShaderStageUPtr shader_stage_create(
		const Renderer3dShaderStageCreateParam& param) override;


	void submit_commands(
		const Renderer3dCommandQueuePtr command_queue) override;


private:
	static void fbo_resource_deleter(
		const GLuint& gl_name) noexcept;

	using FboResource = UniqueResource<GLuint, fbo_resource_deleter>;


	static void rbo_resource_deleter(
		const GLuint& gl_name) noexcept;

	using RboResource = UniqueResource<GLuint, rbo_resource_deleter>;


	using Shaders = std::list<detail::GlShaderUPtr>;
	using ShaderStages = std::list<detail::GlShaderStageUPtr>;


	Renderer3dKind kind_;
	std::string name_;
	std::string description_;

	Renderer3dDeviceInfo device_info_;
	Renderer3dDeviceFeatures device_features_;
	detail::GlDeviceFeatures gl_device_features_;

	int screen_width_;
	int screen_height_;

	Renderer3dAaKind aa_kind_;
	int aa_value_;

	SdlWindowUPtr sdl_window_;
	SdlGlContextUPtr sdl_gl_context_;

	GlExtensionManagerUPtr extension_manager_;
	GlContextUPtr gl_context_;

	FboResource gl_msaa_fbo_;
	RboResource gl_msaa_color_rb_;
	RboResource gl_msaa_depth_rb_;


	void set_name_and_description();


	RboResource renderbuffer_create();

	RboResource renderbuffer_create(
		const int width,
		const int height,
		const int sample_count,
		const GLenum gl_internal_format);

	void renderbuffer_bind(
		const GLuint gl_name);


	FboResource framebuffer_create();

	void framebuffer_bind(
		const GLenum gl_target,
		const GLuint gl_name);

	void framebuffer_blit(
		const int src_width,
		const int src_height,
		const int dst_width,
		const int dst_height,
		const bool is_linear_filter);


	void msaa_color_rb_destroy();

	void msaa_depth_rb_destroy();

	void msaa_fbo_destroy();

	void msaa_framebuffer_destroy();

	void msaa_color_rb_create(
		const int width,
		const int height,
		const int sample_count);

	void msaa_depth_rb_create(
		const int width,
		const int height,
		const int sample_count);

	void msaa_framebuffer_create();


	void framebuffers_destroy();

	void framebuffers_create();

	void framebuffers_blit();

	void framebuffers_bind();


	void aa_disable();

	void msaa_set(
		const int aa_value);


	void command_execute_clear(
		const Renderer3dCommandClear& command);

	void command_execute_culling(
		const Renderer3dCommandCulling& command);

	void command_execute_depth_test(
		const Renderer3dCommandDepthTest& command);

	void command_execute_depth_write(
		const Renderer3dCommandDepthWrite& command);

	void command_execute_blending(
		const Renderer3dCommandBlending& command);

	void command_execute_blending_func(
		const Renderer3dCommandBlendingFunc& command);

	void command_execute_viewport(
		const Renderer3dCommandViewport& command);

	void command_execute_scissor(
		const Renderer3dCommandScissor& command);

	void command_execute_scissor_box(
		const Renderer3dCommandScissorBox& command);

	void command_execute_texture(
		const Renderer3dCommandTexture& command);

	void command_execute_sampler(
		const Renderer3dCommandSampler& command);

	void command_execute_vertex_input(
		const Renderer3dCommandVertexInput& command);

	void command_execute_shader_stage(
		const Renderer3dCommandShaderStage& command);

	void command_execute_shader_var_int32(
		const Renderer3dCommandShaderVarInt32& command);

	void command_execute_shader_var_float32(
		const Renderer3dCommandShaderVarFloat32& command);

	void command_execute_shader_var_vec2(
		const Renderer3dCommandShaderVarVec2& command);

	void command_execute_shader_var_vec4(
		const Renderer3dCommandShaderVarVec4& command);

	void command_execute_shader_var_mat4(
		const Renderer3dCommandShaderVarMat4& command);

	void command_execute_shader_var_sampler_2d(
		const Renderer3dCommandShaderVarSampler2d& command);

	void command_execute_draw_indexed(
		const Renderer3dCommandDrawIndexed& command);
}; // GlRenderer3d


using GlRenderer3dPtr = GlRenderer3d*;
using GlRenderer3dUPtr = std::unique_ptr<GlRenderer3d>;


} // detail
} // bstone


#endif // !BSTONE_DETAIL_GL_RENDERER_3D_INCLUDED
