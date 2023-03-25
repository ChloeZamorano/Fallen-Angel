#include "pch.hpp"

#include "HE/Bina.hpp"
#include <iomanip>

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
	auto file = he::BinaDescriptor::Load("./w9d02_obj_area00.gedit");
	
	std::cout <<
		"w9d02_obj_area00.gedit\n" <<
		"	Values as bytes:\n" <<
		std::hex << std::uppercase;
	std::vector<u8*> ptrs = file.m_Nodes[0].GetPointers((u8*)file.m_Header);
	for(u32 i = 0; i < ptrs.size(); ++i)
		std::cout << "		" <<
			std::setw(8) << std::setfill('0') << *(u32*)ptrs[i] << "\n";
	std::cout << std::dec << std::nouppercase << "EOF";
}
