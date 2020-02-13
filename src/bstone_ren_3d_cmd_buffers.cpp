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
// 3D renderer's command buffer list (implementaion).
//


#include "bstone_precompiled.h"

#include "bstone_ren_3d_cmd_buffers.h"

#include <algorithm>
#include <vector>

#include "bstone_exception.h"


namespace bstone
{
namespace detail
{


// ==========================================================================
// Ren3dCmdBuffersImplException
//

class Ren3dCmdBuffersImplException :
	public Exception
{
public:
	explicit Ren3dCmdBuffersImplException(
		const char* const message)
		:
		Exception{std::string{"[REN_3D_CMD_BUFS] "} + message}
	{
	}
}; // Ren3dCmdBuffersImplException

//
// Ren3dCmdBuffersImplException
// ==========================================================================


// ==========================================================================
// Ren3dCmdBuffersImpl
//

class Ren3dCmdBuffersImpl final :
	public Ren3dCmdBuffers
{
public:
	Ren3dCmdBuffersImpl();

	~Ren3dCmdBuffersImpl() override = default;


	int get_count() const noexcept override;

	Ren3dCmdBufferPtr add(
		const Ren3dCreateCmdBufferParam& param) override;

	void remove(
		const Ren3dCmdBufferPtr buffer) override;

	Ren3dCmdBufferPtr get(
		const int index) override;


private:
	static constexpr auto reserve_count = 16;


	using Buffers = std::vector<Ren3dCmdBufferUPtr>;
	Buffers buffers_;
}; // Ren3dCmdBuffersImpl

using Ren3dCmdBuffersImplPtr = Ren3dCmdBuffersImpl*;
using Ren3dCmdBuffersImplUPtr = std::unique_ptr<Ren3dCmdBuffersImpl>;

//
// Ren3dCmdBuffersImpl
// ==========================================================================


// ==========================================================================
// Ren3dCmdBuffersImpl
//

Ren3dCmdBuffersImpl::Ren3dCmdBuffersImpl()
	:
	buffers_{}
{
	buffers_.reserve(reserve_count);
}

int Ren3dCmdBuffersImpl::get_count() const noexcept
{
	return static_cast<int>(buffers_.size());
}

Ren3dCmdBufferPtr Ren3dCmdBuffersImpl::add(
	const Ren3dCreateCmdBufferParam& param)
{
	auto buffer = Ren3dCmdBufferFactory::create(param);

	buffers_.push_back(std::move(buffer));

	return buffers_.back().get();
}

void Ren3dCmdBuffersImpl::remove(
	const Ren3dCmdBufferPtr buffer)
{
	if (!buffer)
	{
		return;
	}

	static_cast<void>(std::remove_if(
		buffers_.begin(),
		buffers_.end(),
		[buffer](const auto& item)
		{
			return item.get() == buffer;
		}
	));
}

Ren3dCmdBufferPtr Ren3dCmdBuffersImpl::get(
	const int index)
{
	if (index < 0 || index >= get_count())
	{
		throw Ren3dCmdBuffersImplException{"Index out of range."};
	}

	return buffers_[index].get();
}

//
// Ren3dCmdBuffersImpl
// ==========================================================================


} // detail


// ==========================================================================
// Ren3dCmdBuffersFactory
//

Ren3dCmdBuffersUPtr Ren3dCmdBuffersFactory::create()
{
	return std::make_unique<detail::Ren3dCmdBuffersImpl>();
}

//
// Ren3dCmdBuffersFactory
// ==========================================================================


} // bstone
