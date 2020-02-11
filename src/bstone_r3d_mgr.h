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


#ifndef BSTONE_R3D_MGR_INCLUDED
#define BSTONE_R3D_MGR_INCLUDED


#include "bstone_r3d.h"


namespace bstone
{


class R3dMgr
{
protected:
	R3dMgr() = default;


public:
	virtual ~R3dMgr() = default;


	virtual R3dPtr renderer_initialize(
		const R3dCreateParam& param) = 0;
}; // R3dMgr

using R3dMgrUPtr = std::unique_ptr<R3dMgr>;


class R3dMgrFactory
{
public:
	static R3dMgrUPtr create();
}; // R3dMgrFactory


} // bstone


#endif // !BSTONE_R3D_MGR_INCLUDED
