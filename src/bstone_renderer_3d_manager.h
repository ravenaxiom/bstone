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
// 3D renderer manager.
//


#ifndef BSTONE_RENDERER_3D_MANAGER_INCLUDED
#define BSTONE_RENDERER_3D_MANAGER_INCLUDED


#include "bstone_renderer_3d.h"


namespace bstone
{


class Renderer3dManager
{
protected:
	Renderer3dManager() = default;


public:
	virtual ~Renderer3dManager() = default;


	virtual Renderer3dPtr renderer_initialize(
		const Renderer3dCreateParam& param) = 0;
}; // Renderer3dManager

using Renderer3dManagerUPtr = std::unique_ptr<Renderer3dManager>;


class Renderer3dManagerFactory
{
public:
	static Renderer3dManagerUPtr create();
}; // Renderer3dManagerFactory


} // bstone


#endif // !BSTONE_RENDERER_3D_MANAGER_INCLUDED