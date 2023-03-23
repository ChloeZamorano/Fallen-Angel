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

		static Result<cstr, BinaDataNodeDescriptor> Read(BinaNode* basicNode);
	};
}
