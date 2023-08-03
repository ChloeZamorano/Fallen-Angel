#include "pch.hpp"

#include "HE/Bina/Bina.hpp"
#include "Devil/Array.hpp"

using namespace fln;

struct basicInfo
{
	cstr first_name;
	cstr last_name;
	cstr gender;
};

struct entry
{
	basicInfo info;
	u32 id;
	cstr email;
	u8 ip_address[4];
};

struct json
{
	Array<entry, u32> entries;
};

//[{
//  "id": 1,
//  "first_name": "Jeanette",
//  "last_name": "Penddreth",
//  "email": "jpenddreth0@census.gov",
//  "gender": "Female",
//  "ip_address": "26.58.193.2"
//}, {
//  "id": 2,
//  "first_name": "Giavani",
//  "last_name": "Frediani",
//  "email": "gfrediani1@senate.gov",
//  "gender": "Male",
//  "ip_address": "229.179.4.212"
//}, {
//  "id": 3,
//  "first_name": "Noell",
//  "last_name": "Bea",
//  "email": "nbea2@imageshack.us",
//  "gender": "Female",
//  "ip_address": "180.66.162.255"
//}, {
//  "id": 4,
//  "first_name": "Willard",
//  "last_name": "Valek",
//  "email": "wvalek3@vk.com",
//  "gender": "Male",
//  "ip_address": "67.76.188.26"
//}]


i32 main()
{
	auto bina = he::BinaDescriptor::Load("./TestFiles/HE2/RFL/player_common.rfl");
	
	auto members = bina.m_Node->GetMembers();
	auto strings = bina.m_Node->GetStrings();

	auto buf = he::BinaDescriptor::Build(
		members, strings,
		NULL, 0,
		'L');
	
	FILE* file = fopen("./player_common.rfl", "wb");
	if(!file) return 69;
	fwrite(buf.first.get(), 1, buf.second, file);
    fclose(file);

	return 0;
}
