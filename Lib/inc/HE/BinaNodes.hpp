#pragma once
#include "pch.hpp"

#include "Result.tpp"

namespace fln::he
{
	struct BinaNode
	{
		char Signature[4];
		u32 Length;
	};

	const char BinaDataNodeSignature[4] = {'D','A','T','A'};
	struct BinaDataNode
	{
		BinaNode Basic;

		// The non-absolute (relative to the beginning of the Data array below, which is always
		// 0x40 from our findings) offset to the BINA String Table explained below.
		u32 StringTableOffset;
		// The length of the BINA String Table explained below.
		u32 StringTableLength;

		// The length of the BINA Offset Table explained below.
		u32 OffsetTableLength;
		u16 AdditionalDataLength;
		// Just two nulls to pad-out AdditionalDataLength to 4 bytes.
		u16 Padding;
	};

	enum class OffsetMask : u8
	{
		SizeMask	= 0xC0ui8,

		SizeU8		= 0x40ui8,
		SizeU16		= 0x80ui8,
		SizeU32		= 0xC0ui8
	};

	inline OffsetMask operator &(const OffsetMask& left, const OffsetMask& right)
	{
		return (OffsetMask)((u8)left & (u8)right);
	}

	class BinaDataNodeDescriptor
	{
	public:
		BinaDataNode* m_Node;
		// Exact purpose unknown. Seems to always just contain nulls?
		u8* m_AdditionalData;
		// Specific to each type of file. E.G. gedit files contain gedit data.
		u8* m_Data;
		cstr m_StringsTable;
		u8* m_OffsetsTable;

		u64 m_DataLength;

		BinaDataNodeDescriptor(BinaNode* base);

		u32 GetNextOffset(u32 previousPtr, u32& i);
		std::vector<u32> GetOffsets(u32 sizeofBinaHeader);
	};
}
