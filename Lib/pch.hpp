#pragma once

//
// STL headers
//
#include <cstdint>
#include <bitset>

#include <vector>
#include <string>
#include <cstring>

#include <cstdio>
#include <iostream>
#include <iomanip>

#include <filesystem>

#include <stdexcept>

//
// Better types
//
typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef float	f32;
typedef double	f64;

typedef const char* cstr;

//
// Useful macros
//
#define CSTR_TO_SIG32(x) (*((u32*)x))

#define NULL 0

//
// Functions
//
inline void PrintBuffer(u8* ptr, u64 len, cstr indent)
{
	if (len <= UINT32_MAX)
	{
		std::cout << std::hex << std::uppercase <<
			indent << "         | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
			indent << "---------|------------------------------------------------\n" <<
			indent << "00000000 | ";
		for (u64 i = 0; i < len; ++i)
		{
			std::cout <<
				std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';

			if ((i + 1) % 16 == 0 && i != len - 1)
				std::cout << "\n" << indent <<
				std::setw(8) << std::setfill('0') << i + 1 << " | ";
		}
	}
	else
	{
		std::cout << std::hex << std::uppercase <<
			indent << "                 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
			indent << "-----------------|------------------------------------------------\n" <<
			indent << "0000000000000000 | ";
		for (u64 i = 0; i < len; ++i)
		{
			std::cout <<
				std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';

			if ((i + 1) % 16 == 0)
				std::cout << "\n" << indent <<
				std::setw(16) << std::setfill('0') << i + 1 << " | ";
		}
	}
	std::cout << std::dec << std::nouppercase << "\n";
}
