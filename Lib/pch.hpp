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

#define null 0

#define true 1
#define false 0
