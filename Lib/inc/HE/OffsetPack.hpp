#pragma once
#include "pch.hpp"

#include "Devil/Array.hpp"
#include <exception>
#include <memory>

#define HE_OFFSET_TYPE_00 0x00ui8
#define HE_OFFSET_TYPE_06 0x40ui8
#define HE_OFFSET_TYPE_14 0x80ui8
#define HE_OFFSET_TYPE_30 0xC0ui8

#define HE_OFFSET_DATA_MASK_08 0x3Fui8
#define HE_OFFSET_DATA_MASK_16 0x3FFFui16
#define HE_OFFSET_DATA_MASK_32 0x3FFFFFFFui32

#define HE_OFFSET_TYPE_MASK_08 0xC0ui8
#define HE_OFFSET_TYPE_MASK_16 0xC000ui16
#define HE_OFFSET_TYPE_MASK_32 0xC0000000ui32

#define HE_OFFSET_MAX_VALUE_08 0xFCui8
#define HE_OFFSET_MAX_VALUE_16 0xFFFCui16
#define HE_OFFSET_MAX_VALUE_32 0xFFFFFFFCui32

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
			table.emplace_back(static_cast<u8>(offset >> 2) | HE_OFFSET_TYPE_06);
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
		u8 type = HE_OFFSET_TYPE_MASK_08 & *offset;

		switch(type)
		{
		case HE_OFFSET_TYPE_06:
			out = static_cast<size_t>(*offset << 2);
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
			throw std::exception("w h a t");
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
	Array<size_t, u32> UnpackOffsets(const u8* table, u32 tableWidth)
	{
		std::vector<size_t> offsets;

		u32 i = 0;
		size_t offset = 0;
		while(i < tableWidth)
		{
			if(UnpackOffset(offset, &table[i], i))
				offsets.emplace_back(offset);
			else
				break;
		}

		return Array<size_t, u32>(&offsets[0], offsets.size());
	}

	Array<u8, u32> PackOffsets(const Array<VarBuf, u32>& vars)
	{
		std::vector<u8> table;

		for(u32 i = 0; i < vars.Count(); ++i)
		{
			if(i == 0)
				PackOffset(table, 16);
			else
				PackOffset(table, vars[i-1].width);
		}
		table.emplace_back(0);

		return Array<u8, u32>(&table[0], (u32)table.size());
	}
}
