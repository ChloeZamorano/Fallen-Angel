#pragma once
#include "pch.hpp"

#include "Devil/Array.hpp"
#include "HE/OffsetPack.hpp"

namespace fln::he
{
	using namespace dvl;

	struct BinaNode
	{
		constexpr const static char
			Signature[4] = {'D','A','T','A'};
		u32 Length;

		u32 StringTableOffset;
		u32 StringTableLength;

		u32 OffsetTableLength;
		const static u16 AdditionalDataLength = 24;
		const static u16 Padding = 0;
	};

	class BinaNodeDescriptor
	{
	public:
		BinaNode* m_Head;

		// Exact purpose unknown. Seems to always just contain nulls?
		u8* m_AdditionalData;
		// Specific to each type of file. E.G. gedit files contain gedit data.
		u8* m_Data;
		cstr m_StringsTable;
		u8* m_OffsetsTable;

		u64 m_DataLength;

		static std::unique_ptr<BinaNodeDescriptor> New(u8* base);

		u64 GetNextOffset(u32& i);
		u64 GetNextFinalOffset(u64 previousPtr, u32& i);
		std::vector<u64> GetOffsets();
		std::vector<u64> GetFinalOffsets();

		u8* GetNextPointer(u8* previousPtr, u32& i);
		std::vector<u8*> GetPointers(u8* header);

		std::vector<cstr> GetStrings();

		std::vector<std::pair<u8*, u64>> GuessMembers(u8* header);
	};

	enum class OffsetMask : u8
	{
		OFFSET_SIZE_MASK	= 0xC0,

		OFFSET_SIZE_06		= 0x40,
		OFFSET_SIZE_14		= 0x80,
		OFFSET_SIZE_30		= 0xC0
	};

	inline OffsetMask operator &(const OffsetMask& left, const OffsetMask& right)
	{
		return (OffsetMask)((u8)left & (u8)right);
	}

	const  u8 OFFSET_06_DATA_MASK = 0x3F;
	const u16 OFFSET_14_DATA_MASK = 0x3FFF;
	const u32 OFFSET_30_DATA_MASK = 0x3FFF'FFFF;

	struct BinaHeader
	{
		constexpr const static char
			Signature[4] = {'B','I','N','A'};
		constexpr const static char
			Version[3] = {'2','1','0'};
		char EndianFlag = 'L';
		u32 FileSize;
		u16 NodeCount;
		const static u16 Unknown = 0;	// TODO: Look into what's up with this value
	};

	// TODO: Handle endianness
	class BinaDescriptor
	{
	public:
		std::unique_ptr<u8[]> m_FileStart;
			BinaHeader* m_Header;
			std::unique_ptr<BinaNodeDescriptor> m_Node;
			u8* m_VirtualFileEnd;
		u8* m_FileEnd;

		u64 m_FileSize;

		
		static BinaDescriptor Read(std::unique_ptr<u8[]>&& buf, u64 width);

		static BinaDescriptor Load(cstr filePath);

		static std::unique_ptr<u8[]> Build(
			Array<VarBuf, u32> Variables,
			Array<cstr, u32> Strings,
			u8* XData, u64 XDataLen);
	};
}
