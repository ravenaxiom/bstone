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
// 3D renderer's command manager (implementaion).
//


#ifndef BSTONE_DETAIL_RENDERER_3D_COMMAND_MANAGER_INCLUDED
#define BSTONE_DETAIL_RENDERER_3D_COMMAND_MANAGER_INCLUDED


#include "bstone_detail_renderer_3d_command_buffer.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Renderer3dCommandManagerImpl
//

class Renderer3dCommandManagerImpl final :
	public Renderer3dCommandManager
{
public:
	Renderer3dCommandManagerImpl();

	~Renderer3dCommandManagerImpl() override;


	int buffer_get_count() const noexcept override;

	bstone::Renderer3dCommandBufferPtr buffer_add(
		const Renderer3dCommandManagerBufferAddParam& param) override;

	void buffer_remove(
		bstone::Renderer3dCommandBufferPtr buffer) override;

	bstone::Renderer3dCommandBufferPtr buffer_get(
		const int index) override;


private:
	static constexpr auto reserved_buffer_count = 8;


	using Buffers = std::vector<detail::RendererCommandBufferUPtr>;


	Buffers buffers_;


	static void validate_param(
		const Renderer3dCommandManagerBufferAddParam& param);
}; // Renderer3dCommandManagerImpl

using RendererCommandManagerImplPtr = Renderer3dCommandManagerImpl*;
using RendererCommandManagerImplUPtr = std::unique_ptr<Renderer3dCommandManagerImpl>;

//
// Renderer3dCommandManagerImpl
// ==========================================================================


} // detail
} // bstone


#endif // !BSTONE_DETAIL_RENDERER_3D_COMMAND_MANAGER_INCLUDED
