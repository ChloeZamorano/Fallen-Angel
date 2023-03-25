#include "HE/BinaNodes.hpp"

namespace fln::he
{
	BinaDataNodeDescriptor::BinaDataNodeDescriptor(BinaNode* basicNode)
	{
		m_Node = (BinaDataNode*)basicNode;
		m_AdditionalData = (((u8*)basicNode) + sizeof(BinaDataNode));
		m_Data = m_AdditionalData + m_Node->AdditionalDataLength;
		m_StringsTable = (cstr)m_Data + m_Node->StringTableOffset;
		m_OffsetsTable = (u8*)m_StringsTable + m_Node->StringTableLength;

		m_DataLength =
			m_Node->Basic.Length - (sizeof(BinaDataNode)
			+ m_Node->AdditionalDataLength);
	}

	u64 BinaDataNodeDescriptor::GetNextOffset(u64 previousPtr, u32& i)
	{
		auto ptr = (u8*)(m_OffsetsTable + i);
		u8 b1 = ptr[0] & OFFSET_06_DATA_MASK;
		u8 b2 = ptr[1];
		u8 b3 = ptr[2];
		u8 b4 = ptr[3];
		u64 offset = 0;
		switch (*(OffsetMask*)ptr & OffsetMask::SizeMask)
		{
		case OffsetMask::SizeU8:
			offset = (u32)(b1 << 2);
			++i;
			return previousPtr + offset;
		case OffsetMask::SizeU16:
			offset = (u32)(
				((b1 << 8) | b2)
				<< 2);
			i += 2;
			return previousPtr + offset;
		case OffsetMask::SizeU32:
			offset = (u32)(
				((b1 << 24) | (b2 << 16) | (b3 << 8) | b4)
				<< 2);
			i += 4;
			return previousPtr + offset;
		default:
			return null;
		}
	}

	std::vector<u64> BinaDataNodeDescriptor::GetOffsets()
	{
		std::vector<u64> output;
		u32 i = 0;
		while(i < m_Node->OffsetTableLength)
		{
			u64 prevptr = output.size() == 0 ?
				64 :
				output[output.size()-1];

			u64 ptr = GetNextOffset(prevptr, i);
			if (ptr == null)
				return output;
			output.emplace_back(ptr);
		}
		return output; // This should never happen.
	}

	u8* BinaDataNodeDescriptor::GetNextPointer(u8* previousPtr, u32& i)
	{
		return (u8*)GetNextOffset((u64)previousPtr, i);
	}

	std::vector<u8*> BinaDataNodeDescriptor::GetPointers(u8* header)
	{
		std::vector<u8*> output;
		u32 i = 0;
		while(i < m_Node->OffsetTableLength)
		{
			u8* prevptr = output.size() == 0 ?
				header + 64 :
				output[output.size()-1];

			u8* ptr = GetNextPointer(prevptr, i);
			if (ptr == null)
				return output;
			output.emplace_back(ptr);
		}
		return output; // This should never happen.
	}
}
