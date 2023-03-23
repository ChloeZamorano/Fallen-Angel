#include "pch.hpp"

#include "HE/Bina.hpp"

using namespace fln;

i32 main()
{
	Result<cstr, he::BinaDescriptor> fileRes =
		he::BinaDescriptor::Load(
		"./w9d02_obj_area00.gedit");

	he::BinaDescriptor file = fileRes.Expect();
	
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
				"		AdditionalData:" << "\n" <<
				"			00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
				"			-----------------------------------------------\n" <<
				std::hex << std::uppercase << "			";
			for(u32 j = 0; j < nodeDesc.m_Node->AdditionalDataLength; ++j)
			{
				std::cout << std::setw(2) << std::setfill('0') <<
					(u32)nodeDesc.m_AdditionalData[j] << " ";
				
				if((j+1)%16 == 0)
				{
					std::cout << "\n			";
				}
			}
			std::cout <<
				"\n		Data:" << "\n" <<
				"			00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
				"			-----------------------------------------------\n" <<
				"			";
			for(u32 j = 0; j < nodeDesc.m_DataLength; ++j)
			{
				std::cout << std::setw(2) << std::setfill('0') <<
					(u32)nodeDesc.m_Data[j] << " ";
				
				if((j+1)%16 == 0)
				{
					std::cout << "\n			";
				}
			}
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
				"		OffsetsTable:" << "\n" <<
				std::hex << std::uppercase;
			pos = 0;
			while(pos < nodeDesc.m_Node->OffsetTableLength)
			{
				auto ptr = (u32*)(nodeDesc.m_OffsetsTable + pos);
				auto lenptr = (u8*)ptr;
				//std::cout << std::setw(8) << std::setfill('0') << *ptr << "\n";
				switch((*lenptr) & 0xC0)
				{
				case 0x40:
					{
						std::cout << "			" << std::setw(8) << std::setfill('0') <<
							(u32)((*lenptr & 0x3F) << 2) << "\n";
						++pos;
					}
					break;
				case 0x80:
					{
						std::cout << "			" << std::setw(8) << std::setfill('0') <<
							((*(u16*)ptr & 0x3FFF) << 2) << "\n";
						pos += 2;
					}
					break;
				case 0xC0:
					{
						std::cout << "			" << std::setw(8) << std::setfill('0') <<
							((*ptr & 0x3FFF'FFFF) << 2) << "\n";
						pos += 4;
					}
					break;
				default:
					pos += 4;
					break;
				}
			}
			std::cout <<
				std::dec << std::nouppercase <<
				"		Values:" << "\n";
			
		}
		else
			throw std::logic_error("Node not implemented.");
	}
}
