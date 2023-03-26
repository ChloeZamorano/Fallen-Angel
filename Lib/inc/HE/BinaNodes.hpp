#pragma once
#include "pch.hpp"

namespace fln::he
{
	struct BinaNode
	{
		char Signature[4];
		u32 Length;
	};

#define ENDIAN_LITTLE // Fix this crap
	const u32 BINA_NODE_TYPE_DATA =
#ifdef ENDIAN_LITTLE
		0x41'54'41'44;
#else
		0x44'41'54'41;
#endif

	class BinaNodeDescriptor
	{
	public:
		BinaNode* m_Head;

		static std::shared_ptr<BinaNodeDescriptor> New(BinaNode* base);
	};

	struct BinaDataNode
	{
		u32 StringTableOffset;
		u32 StringTableLength;

		u32 OffsetTableLength;
		u16 AdditionalDataLength;
		u16 Padding;
	};

	class BinaDataNodeDescriptor : public BinaNodeDescriptor
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

		static std::shared_ptr<BinaDataNodeDescriptor> New(BinaNode* base);

		u64 GetNextOffset(u64 previousPtr, u32& i);
		std::vector<u64> GetOffsets();

		u8* GetNextPointer(u8* previousPtr, u32& i);
		std::vector<u8*> GetPointers(u8* header);

		std::vector<cstr> GetStrings();
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

	const  u8 OFFSET_06_DATA_MASK = 0x3Fui8;
	const u16 OFFSET_14_DATA_MASK = 0x3FFFui16;
	const u32 OFFSET_30_DATA_MASK = 0x3FFF'FFFFui32;
}
