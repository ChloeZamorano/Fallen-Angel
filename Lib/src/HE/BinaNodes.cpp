#include "HE/BinaNodes.hpp"

namespace fln::he
{
	std::shared_ptr<BinaNodeDescriptor> BinaNodeDescriptor::New(BinaNode* base)
	{
		switch(*(u32*)base->Signature)
		{
		case BINA_NODE_TYPE_DATA:
			return BinaDataNodeDescriptor::New(base);
		default:
			std::stringstream ss;
			ss << "Bina node type \"" <<
				base->Signature[0] <<
				base->Signature[1] <<
				base->Signature[2] <<
				base->Signature[3] <<
				"\" not yet implemented!";
			throw std::logic_error(ss.str());
		}
	}

	std::shared_ptr<BinaDataNodeDescriptor> BinaDataNodeDescriptor::New(BinaNode* base)
	{
		auto output = std::make_shared<BinaDataNodeDescriptor>();
		output->m_Head = base;
		output->m_Node = (BinaDataNode*)((u8*)base + sizeof(BinaNode));
		output->m_AdditionalData = (((u8*)output->m_Node) + sizeof(BinaDataNode));
		output->m_Data = output->m_AdditionalData + output->m_Node->AdditionalDataLength;
		output->m_StringsTable = (cstr)output->m_Data + output->m_Node->StringTableOffset;
		output->m_OffsetsTable = (u8*)output->m_StringsTable + output->m_Node->StringTableLength;

		output->m_DataLength =
			output->m_Head->Length -(sizeof(BinaDataNode)
			+ output->m_Node->AdditionalDataLength);

		return output;
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
			return NULL;
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
			if (ptr == NULL)
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
			if (ptr == NULL)
				return output;
			output.push_back(ptr);
		}
		return output; // This should never happen.
	}

	std::vector<cstr> BinaDataNodeDescriptor::GetStrings()
	{
		std::vector<cstr> output;

		u32 pos = 0;
		while (pos < m_Node->StringTableLength)
		{
			if (m_StringsTable[pos] == NULL)
				break;

			output.emplace_back(m_StringsTable + pos);
			pos += strlen(m_StringsTable + pos) + 1;
		}
		return output;
	}
}
