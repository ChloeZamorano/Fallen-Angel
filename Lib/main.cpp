#include "pch.hpp"

#include "HE/HE2/Reflection.hpp"

using namespace fln;

i32 main()
{
	auto file = he2::Reflection::Load("./player_common.rfl").Expect();

	std::vector<cstr> str = file.GetStrings();
	std::vector<u8*> ptr = file.GetPointers();

	std::cout <<
		"player_common.rfl\n" <<
		"	Strings:\n";
	for (u32 i = 0; i < str.size(); ++i)
		std::cout << "		" << str[i] << "\n";

	std::cout <<
		"	Bytes:\n" << std::hex << std::uppercase;
	for (u32 i = 0; i < ptr.size(); ++i)
		std::cout << "		" << std::setfill('0') << std::setw(2) << (u32)*ptr[i] << "\n";

	std::cout << std::dec << std::nouppercase << "EOF";

	return 0;
}
