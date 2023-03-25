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

	// TODO: Documentation says to bit shift the values to the left by 2
	// but that causes a segfault after dereferencing a few offsets. Got to
	// figure out what funkiness is going on there; for now let's see
	// if it works without it.
	u32 BinaDataNodeDescriptor::GetNextOffset(u32 previousPtr, u32& i)
	{
		auto ptr = (u8*)(m_OffsetsTable + i);
		u32 offset = 0;
		switch (*(OffsetMask*)ptr & OffsetMask::SizeMask)
		{
		case OffsetMask::SizeU8:
			++i;
			offset = ((*(u8*)ptr) & 0x3Fui8) << 0;
			return previousPtr + offset;
		case OffsetMask::SizeU16:
			i += 2;
			offset = ((*(u16*)ptr) & 0x3FFFui16) << 0;
			return previousPtr + offset;
		case OffsetMask::SizeU32:
			i += 4;
			offset = ((*(u32*)ptr) & 0x3FFF'FFFFui32) << 0;
			return previousPtr + offset;
		default:
			return null;
		}
	}

	std::vector<u32> BinaDataNodeDescriptor::GetOffsets(u32 sizeofBinaHeader)
	{
		std::vector<u32> output;
		u32 i = 0;
		while(i < m_Node->OffsetTableLength)
		{
			u32 prevptr = output.size() == 0 ?
				sizeofBinaHeader :
				output[output.size()-1];

			u32 ptr = GetNextOffset(prevptr, i);
			if (ptr == null)
				return output;
			output.push_back(ptr);
		}
		return output; // This should never happen.
	}
}
