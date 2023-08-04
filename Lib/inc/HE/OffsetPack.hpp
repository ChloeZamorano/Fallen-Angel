#pragma once
#include "pch.hpp"

#include "Devil/Array.inl"

#define HE_BINA_FIRST_OFFSET 0

#define HE_OFFSET_TYPE_00 ((u8)0x00)
#define HE_OFFSET_TYPE_06 ((u8)0x40)
#define HE_OFFSET_TYPE_14 ((u8)0x80)
#define HE_OFFSET_TYPE_30 ((u8)0xC0)

#define HE_OFFSET_DATA_MASK_08 ((u8)0x3F)
#define HE_OFFSET_DATA_MASK_16 ((u16)0x3FFF)
#define HE_OFFSET_DATA_MASK_32 ((u32)0x3FFFFFFF)

#define HE_OFFSET_TYPE_MASK_08 ((u8)0xC0)
#define HE_OFFSET_TYPE_MASK_16 ((u16)0xC000)
#define HE_OFFSET_TYPE_MASK_32 ((u32)0xC0000000)

#define HE_OFFSET_MAX_VALUE_08 ((u8)0xFC)
#define HE_OFFSET_MAX_VALUE_16 ((u16)0xFFFC)
#define HE_OFFSET_MAX_VALUE_32 ((u32)0xFFFFFFFC)

//
// TODO: Use your own vector class for API consistency with the Array class.
// TODO: Name these functions better.
//
namespace fln::he
{
	struct VarBuf
	{
		u8* buffer;
		size_t width;
	};

	inline void PackOffset(std::vector<u8>& table, const size_t& offset)
	{
		if(offset <= HE_OFFSET_MAX_VALUE_08)
		{
			table.push_back(static_cast<u8>(offset >> 2) | HE_OFFSET_TYPE_06);
		}
		else if(offset <= HE_OFFSET_MAX_VALUE_16)
		{
        	u16 value = FLN_SWAP_16(static_cast<u16>(offset >> 2));
        	u8* ptr = reinterpret_cast<u8*>(&value);
			ptr[0] |= HE_OFFSET_TYPE_14;
        	table.insert(table.end(), ptr, ptr + sizeof(value));
		}
		else if(offset <= HE_OFFSET_MAX_VALUE_32)
		{
        	u32 value = static_cast<u32>(offset >> 2);
        	u8* ptr = reinterpret_cast<u8*>(&value);
			ptr[0] |= HE_OFFSET_TYPE_30;
        	table.insert(table.end(), ptr, ptr + sizeof(value));
		}
		else throw std::runtime_error(
			"fln::he::OffsetsTable::OffsetsTable(const u32*, const u32&):"
			"Encountered an offset with a value higher than (2^30)-1 "
			"which cannot fit into the offset format used by data nodes "
			"in Bina files.");
	}

	inline bool UnpackOffset(size_t& out, const u8* offset, u32& i)
	{
		u8 type = *offset & HE_OFFSET_TYPE_MASK_08;

		switch(type)
		{
		case HE_OFFSET_TYPE_06:
			out = static_cast<size_t>((*offset & HE_OFFSET_DATA_MASK_08) << 2);
			break;

		case HE_OFFSET_TYPE_14:
			out = static_cast<size_t>(
				HE_OFFSET_DATA_MASK_16 & FLN_SWAP_16(*reinterpret_cast<const u16*>(offset))
				) << 2;
			break;

		case HE_OFFSET_TYPE_30:
			out = static_cast<size_t>(
				HE_OFFSET_DATA_MASK_32 & FLN_SWAP_32(*reinterpret_cast<const u32*>(offset))
				) << 2;
			break;

		case HE_OFFSET_TYPE_00:
			return false;

		default:
			throw std::runtime_error("w h a t");
			break;
		}
		i += type >> 6;
		return true;
	}

	//
	// This function takes in a raw C-style buffer because in the intended usecase, the table will
	// not come from an array anyway, it will come from the middle of a file buffer, and I don't
	// want to implement another array class for referencing the middle of a buffer like that, so
	// raw C-style buffer it is, or I guess array since the pointer is to a byte, whatever.
	//
	inline Array<size_t, u32> UnpackOffsets(const u8* table, const u32 tableWidth)
	{
		std::vector<size_t> offsets;

		u32 i = 0;
		size_t offset = 0;
		while(i < tableWidth)
		{
			if(UnpackOffset(offset, &table[i], i))
				offsets.push_back(offset);
			else
				break;
		}

		return Array<size_t, u32>(&offsets[0], offsets.size());
	}

	inline Array<u8, u32> PackOffsets(const Array<VarBuf, u32>& vars, const Array<u32, u32>& diffs)
	{
		std::vector<u8> table;
		
		for(u32 i = 0; i < vars.Count(); ++i)
		{
			if(i == 0)
				PackOffset(table, HE_BINA_FIRST_OFFSET);
			else
				PackOffset(table, vars[i-1].width + diffs[i-1]);
		}
		table.push_back(0);

		return Array<u8, u32>(&table[0], (u32)table.size());
	}
}
