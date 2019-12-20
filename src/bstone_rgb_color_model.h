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
// Types of RGB color model.
//


#ifndef BSTONE_RGB_COLOR_MODEL_INCLUDED
#define BSTONE_RGB_COLOR_MODEL_INCLUDED


#include <cstdint>

#include <type_traits>

#include "glm/glm.hpp"


namespace bstone
{


// ==========================================================================
// Rgb8
//

class Rgb8
{
public:
	std::uint8_t r_;
	std::uint8_t g_;
	std::uint8_t b_;


	Rgb8();

	Rgb8(
		const std::uint8_t r,
		const std::uint8_t g,
		const std::uint8_t b);


	std::uint8_t& get(
		const int index);

	const std::uint8_t& get(
		const int index) const;


	std::uint8_t& operator[](
		const int index);

	const std::uint8_t& operator[](
		const int index) const;
}; // Rgb8

using Rgb8Ptr = Rgb8*;
using Rgb8CPtr = const Rgb8*;

//
// Rgb8
// ==========================================================================


// ==========================================================================
// Rgba8
//

class Rgba8
{
public:
	std::uint8_t r_;
	std::uint8_t g_;
	std::uint8_t b_;
	std::uint8_t a_;


	Rgba8();

	Rgba8(
		const std::uint8_t r,
		const std::uint8_t g,
		const std::uint8_t b,
		const std::uint8_t a);


	std::uint8_t& get(
		const int index);

	const std::uint8_t& get(
		const int index) const;


	std::uint8_t& operator[](
		const int index);

	const std::uint8_t& operator[](
		const int index) const;


	void reset();


	// Averages two colors and premultiplies alpha.
	static Rgba8 average_pa(
		const Rgba8 color_0,
		const Rgba8 color_1);

	// Averages four colors and premultiplies alpha.
	static Rgba8 average_pa(
		const Rgba8 color_0,
		const Rgba8 color_1,
		const Rgba8 color_2,
		const Rgba8 color_3);
}; // Rgba8

using Rgba8Ptr = Rgba8*;
using Rgba8CPtr = const Rgba8*;


bool operator==(
	const Rgba8& lhs,
	const Rgba8& rhs);

bool operator!=(
	const Rgba8& lhs,
	const Rgba8& rhs);

//
// Rgba8
// ==========================================================================


} // bstone


#endif // !BSTONE_RGB_COLOR_MODEL_INCLUDED
