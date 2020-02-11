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


#ifndef BSTONE_DETAIL_R3D_GL_INCLUDED
#define BSTONE_DETAIL_R3D_GL_INCLUDED


#include <array>
#include <list>
#include <vector>

#include "bstone_r3d.h"
#include "bstone_r3d_cmd_buffer.h"
#include "bstone_unique_resource.h"

#include "bstone_detail_r3d_gl_buffer.h"
#include "bstone_detail_r3d_gl_extension_mgr.h"
#include "bstone_detail_r3d_gl_shader.h"
#include "bstone_detail_r3d_gl_shader_stage.h"
#include "bstone_detail_r3d_gl_utils.h"
#include "bstone_detail_r3d_gl_context.h"


namespace bstone
{
namespace detail
{


class R3dGl final :
	public R3d
{
public:
	R3dGl(
		const R3dCreateParam& param);

	~R3dGl() override;


	R3dKind get_kind() const noexcept override;

	const std::string& get_name() const noexcept override;

	const std::string& get_description() const noexcept override;


	const R3dDeviceFeatures& device_get_features() const noexcept override;

	const R3dDeviceInfo& device_get_info() const noexcept override;


	void window_set_mode(
		const R3dWindowSetModeParam& param) override;

	void window_set_title(
		const std::string& title_utf8) override;

	void window_show(
		const bool is_visible) override;


	bool vsync_get() const noexcept override;

	void vsync_set(
		const bool is_enabled) override;


	void aa_set(
		const R3dAaKind aa_kind,
		const int aa_value) override;


	void present() override;


	R3dBufferUPtr buffer_create(
		const R3dBufferCreateParam& param) override;

	R3dTexture2dUPtr texture_2d_create(
		const R3dTexture2dCreateParam& param) override;

	R3dSamplerUPtr sampler_create(
		const R3dSamplerCreateParam& param) override;

	R3dVertexInputUPtr vertex_input_create(
		const R3dVertexInputCreateParam& param) override;

	R3dShaderUPtr shader_create(
		const R3dShaderCreateParam& param) override;

	R3dShaderStageUPtr shader_stage_create(
		const R3dShaderStageCreateParam& param) override;


	void submit_commands(
		const R3dCmdQueuePtr command_queue) override;


private:
	static void fbo_resource_deleter(
		const GLuint& gl_name) noexcept;

	using FboResource = UniqueResource<GLuint, fbo_resource_deleter>;


	static void rbo_resource_deleter(
		const GLuint& gl_name) noexcept;

	using RboResource = UniqueResource<GLuint, rbo_resource_deleter>;


	using Shaders = std::list<R3dGlShaderUPtr>;
	using ShaderStages = std::list<R3dGlShaderStageUPtr>;


	R3dKind kind_;
	std::string name_;
	std::string description_;

	R3dDeviceInfo device_info_;
	R3dDeviceFeatures device_features_;
	R3dGlDeviceFeatures gl_device_features_;

	int screen_width_;
	int screen_height_;

	R3dAaKind aa_kind_;
	int aa_value_;

	SdlWindowUPtr sdl_window_;
	SdlGlContextUPtr sdl_gl_context_;

	R3dGlExtensionMgrUPtr extension_manager_;
	R3dGlContextUPtr gl_context_;

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
		const R3dCmdClear& command);

	void command_execute_culling(
		const R3dCmdCulling& command);

	void command_execute_depth_test(
		const R3dCmdDepthTest& command);

	void command_execute_depth_write(
		const R3dCmdDepthWrite& command);

	void command_execute_blending(
		const R3dCmdBlending& command);

	void command_execute_blending_func(
		const R3dCmdBlendingFunc& command);

	void command_execute_viewport(
		const R3dCmdViewport& command);

	void command_execute_scissor(
		const R3dCmdScissor& command);

	void command_execute_scissor_box(
		const R3dCmdScissorBox& command);

	void command_execute_texture(
		const R3dCmdTexture& command);

	void command_execute_sampler(
		const R3dCmdSampler& command);

	void command_execute_vertex_input(
		const R3dCmdVertexInput& command);

	void command_execute_shader_stage(
		const R3dCmdShaderStage& command);

	void command_execute_shader_var_int32(
		const R3dCmdShaderVarInt32& command);

	void command_execute_shader_var_float32(
		const R3dCmdShaderVarFloat32& command);

	void command_execute_shader_var_vec2(
		const R3dCmdShaderVarVec2& command);

	void command_execute_shader_var_vec4(
		const R3dCmdShaderVarVec4& command);

	void command_execute_shader_var_mat4(
		const R3dCmdShaderVarMat4& command);

	void command_execute_shader_var_sampler_2d(
		const R3dCmdShaderVarSampler2d& command);

	void command_execute_draw_indexed(
		const R3dCmdDrawIndexed& command);
}; // R3dGl


using R3dGlPtr = R3dGl*;
using R3dGlUPtr = std::unique_ptr<R3dGl>;


} // detail
} // bstone


#endif // !BSTONE_DETAIL_R3D_GL_INCLUDED
