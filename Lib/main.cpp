#include "pch.hpp"

#include "HE/Bina/Bina.hpp"
#include "Devil/Log.hpp"

using namespace fln;
using namespace dvl;

i32 main()
{
	FUCK_TIM_COOK();
	//auto file = he::BinaDescriptor::Load("./TestFiles/HE2/RFL/player_common.rfl");

	u8 sn = 69;
	u8 array[7]
	{
		'A',
		58,
		'P',
		'P',
		255,
		0,
		'E',
	};
	u32 inty = 420;
	std::vector<std::pair<u8*, u64>> var
	{
		{ (u8*)&sn, sizeof(sn) },
		{ (u8*)&array, sizeof(array) },
		{ (u8*)&inty, sizeof(inty) },
	};
	std::vector<cstr> str
	{
		"Hi hello there :3",
		"Cutie Clyde",
		"God he's hot",
	};
	cstr secret = "This is totally not a super secret message :0";

	auto bina = he::BinaDescriptor::Build(
		var,
		str,
		(u8*)secret, strlen(secret));

	FILE* file = fopen("./nstdout.bin", "wb");
	if(!file) return 69;
	fwrite(bina.m_Header, 1, bina.m_FileSize, file);
    fclose(file);

	return 0;
}
