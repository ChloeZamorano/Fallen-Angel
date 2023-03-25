#include "pch.hpp"

#include "HE/Bina.hpp"

using namespace fln;

inline void PrintBuffer(u8* ptr, u64 len, cstr indent)
{
	if(len <= UINT32_MAX)
	{
		std::cout << std::hex << std::uppercase <<
			indent << "         | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
			indent << "---------|------------------------------------------------\n" <<
			indent << "00000000 | ";
		for (u64 i = 0; i < len; ++i)
		{
			std::cout <<
				std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';

			if ((i + 1) % 16 == 0 && i != len-1)
				std::cout << "\n" << indent <<
				std::setw(8) << std::setfill('0') << i+1 << " | ";
		}
	}
	else
	{
		std::cout << std::hex << std::uppercase <<
			indent << "                 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
			indent << "-----------------|------------------------------------------------\n" <<
			indent << "0000000000000000 | ";
		for (u64 i = 0; i < len; ++i)
		{
			std::cout <<
				std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';

			if ((i + 1) % 16 == 0)
				std::cout << "\n" << indent <<
				std::setw(16) << std::setfill('0') << i+1 << " | ";
		}
	}
	std::cout << std::dec << std::nouppercase << "\n";
}

i32 main()
{
	he::BinaDescriptor file =
		he::BinaDescriptor::Load(
		"./w9d02_obj_area00.gedit");
	
	std::string sig;
	sig.assign(file.m_Header->Signature, file.m_Header->Signature + 4);
	std::string ver;
	ver.assign(file.m_Header->Version, file.m_Header->Version + 3);
	std::cout <<
		"w9d02_obj_area00.gedit\n" <<
		"	Signature: " << sig << "\n" <<
		"	Version:   " << ver << "\n" <<
		"	Endian:    " << file.m_Header->EndianFlag << "\n" <<
		"	FileSize:  " << file.m_Header->FileSize << "\n" <<
		"	NodeCount: " << file.m_Header->NodeCount << "\n" <<
		"	Uknown:    " << file.m_Header->Unknown << "\n" <<
		"	Nodes:\n";

	for(u32 i = 0; i < file.m_Nodes.size(); ++i)
	{
		if(*(u32*)file.m_Nodes[i].m_Node->Basic.Signature == *(u32*)he::BinaDataNodeSignature)
		{
			he::BinaDataNode* node = file.m_Nodes[i].m_Node;
			he::BinaDataNodeDescriptor nodeDesc = file.m_Nodes[i];
			std::cout <<
				"		Signature:            DATA\n" <<
				"		Length:               " << node->Basic.Length << "\n" <<
				"		StringTableOffset:    " << node->StringTableOffset << "\n" <<
				"		StringTableLength:    " << node->StringTableLength << "\n" <<
				"		OffsetTableLength:    " << node->OffsetTableLength << "\n" <<
				"		AdditionalDataLength: " << node->AdditionalDataLength << "\n" <<
				"		Padding:              " << node->Padding << "\n" <<
				"		AdditionalData:\n";
			PrintBuffer(
				nodeDesc.m_AdditionalData,
				nodeDesc.m_Node->AdditionalDataLength,
				"			");
			std::cout <<
				"		Data:\n";
			PrintBuffer(
				nodeDesc.m_Data,
				nodeDesc.m_DataLength,
				"			");

			u64 pos = 0;
			std::cout <<
				std::dec << std::nouppercase <<
				"\n		StringsTable:" << "\n";
			while(pos < nodeDesc.m_Node->StringTableLength/2)
			{
				if(*(u32*)(nodeDesc.m_StringsTable+pos-1) == 0)
					break;
				std::cout <<
					"			" << nodeDesc.m_StringsTable+pos << "\n";
				pos += strlen(nodeDesc.m_StringsTable+pos)+1;
			}
			std::cout <<
				"		OffsetsTable:" << "\n";
				PrintBuffer(
					nodeDesc.m_OffsetsTable,
					nodeDesc.m_Node->OffsetTableLength,
					"			");

			std::cout << std::hex << std::uppercase <<
				"		Values as u32's:\n";
			std::vector<u32> ptrs = file.m_Nodes[i].
				GetOffsets(64);
			for(u32 j = 0; j < ptrs.size(); ++j)
			{
				std::cout <<
					"			" << std::setw(8) << std::setfill('0') <<
					*(u32*)(ptrs[j]+((u8*)file.m_Header)) << "\n";
			}
			std::cout << "EOF";
		}
		else throw std::logic_error("Node not implemented.");
	}
}
