#pragma once
#include "pch.hpp"

namespace fln::he2
{
	struct Tag
	{
		u64 Padding;
		u64 TypeOffset;
		u64 DataLength;
		u64 DataOffset;
	};

	struct RangeSpawning
	{
		f32 RangeIn;
		f32 RangeOut;
	};

	class TagDescriptor
	{
    public:
		Tag* m_Tag;
		u8* m_Data;
	};
}
