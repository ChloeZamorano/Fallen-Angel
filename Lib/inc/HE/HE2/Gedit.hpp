#pragma once
#include "pch.hpp"

#include "HE/Bina.hpp"
#include "HE/HE2/Object.hpp"

namespace fln::he2
{
	using namespace fln::he;

	struct GeditHeader
	{
		u64 Padding[2];

		u64 ObjectOffsetTableOffset;
		u64 ObjectCount;

		// Probably unused, appears to always be set to ObjectCount.
		// Can't imagine it being anything other than a vector's
		// capacity.
		u64 ObjectCapacity;
		u64 Padding1;
	};

	class GeditDescriptor
	{
	public:
		BinaDescriptor m_Bina;
		
		GeditHeader* m_Header;
		u64* m_ObjectOffsetTable;
		Object* m_Objects;

		static GeditDescriptor Load(cstr filePath);
	};
}
