//
// STL headers
//
#include <cstdint>
#include <bitset>

#include <vector>
#include <string>
#include <cstring>

#include <memory>
#include <cstdio>
#include <iostream>
#include <iomanip>

#include <filesystem>

#include <stdexcept>

//
// Library headers we want everywhere
//
#include "Devil/Log.hpp"

//
// Typedefs
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
// Macros
//
#if defined(WINDOWS)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define DVL_TRACE_(...)         dvl::Log::Logger()->trace(__VA_ARGS__)
#define DVL_INFO_(...)          dvl::Log::Logger()->info(__VA_ARGS__)
#define DVL_WARN_(...)          dvl::Log::Logger()->warn(__VA_ARGS__)
#define DVL_ERROR_(...)         dvl::Log::Logger()->error(__VA_ARGS__)
#define DVL_CRITICAL_(...)      dvl::Log::Logger()->critical(__VA_ARGS__)

#define DVL_BREAK() __builtin_trap()

#if defined(DEBUG)
	#define DVL_ASSERT(type, check, msg, ...)				\
	{														\
		if(!(check))										\
		{													\
			DVL_##type##_(msg, __VA_ARGS__);				\
			DVL_BREAK();									\
			DVL_WARN_(										\
				"{0} {1}: Ignored failed assert,"			\
				"expect bad behaviour from the program.",	\
				__FILENAME__, __LINE__);					\
		}													\
	}
#elif defined(RELEASE)
	#define DVL_ASSERT(type, check, msg, ...)				\
	{														\
		if(!(check))										\
		{													\
			DVL_##type##_(msg, __VA_ARGS__);				\
			DVL_WARN_("{0} {1}: Assertion failures do not "	\
				"break in release builds, expect bad "		\
				"behaviour from the program.",				\
				__FILENAME__, __LINE__);					\
		}													\
	}
#else
	#define DVL_ASSERT(...)
#endif

#define CSTR_TO_SIG32(x) (*((u32*)x))

#define NULL 0

//
// Functions
//
#define ADLER32(data, length)							\
({														\
	unsigned int adlerA = 1;							\
	unsigned int adlerB = 0;							\
	const unsigned char* ptr =							\
		reinterpret_cast<const unsigned char*>(data);	\
	const unsigned char* end = ptr + length;			\
														\
	while (ptr < end)									\
	{													\
		adlerA = (adlerA + *ptr++) % 65521;				\
		adlerB = (adlerB + adlerA) % 65521;				\
	}													\
														\
	(adlerB << 16) | adlerA;							\
})

#define FLN_SWAP_16(x) ((x << 8) | (x >> 8))
#define FLN_SWAP_32(x) (((x & 0xFF) << 24) | ((x & 0xFF00) << 8) |	((x & 0xFF0000) >> 8) | ((x >> 24) & 0xFF))
#define FLN_SWAP_64(x) (((x & 0xFFULL) << 56) | ((x & 0xFF00ULL) << 40) | ((x & 0xFF0000ULL) << 24) | ((x & 0xFF000000ULL) << 8) | ((x & 0xFF00000000ULL) >> 8) | ((x & 0xFF0000000000ULL) >> 24) | ((x & 0xFF000000000000ULL) >> 40) | ((x >> 56) & 0xFFULL))


#ifdef __apple_build_version__
	#define FUCK_TIM_COOK() if(rand() > 420){ while(true) (*(u64*)rand()) = 69; }
#else
	#define FUCK_TIM_COOK() {}
#endif
