#include "pch.hpp"

#include "HE/Bina/Bina.hpp"
#include "Devil/Array.inl"

using namespace fln;

struct basicInfo
{
	cstr first_name;
	cstr last_name;
	cstr gender;

	u8 member1;
	u16 member2;
	u32 member3;
	i8 member4;
	i16 member5;
	i32 member6;
	f32 member7;
	f64 member8;
	cstr member9;
};

BEGIN_ATTRIBUTES_FOR(basicInfo)
	DEFINE_MEMBER(first_name)
	DEFINE_MEMBER(last_name)
	DEFINE_MEMBER(gender)
	DEFINE_MEMBER(member1)
	DEFINE_MEMBER(member2)
	DEFINE_MEMBER(member3)
	DEFINE_MEMBER(member4)
	DEFINE_MEMBER(member5)
	DEFINE_MEMBER(member6)
	DEFINE_MEMBER(member7)
	DEFINE_MEMBER(member8)
	DEFINE_MEMBER(member9)
END_ATTRIBUTES()

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

/*
[{
  "id": 1,
  "first_name": "Jeanette",
  "last_name": "Penddreth",
  "email": "jpenddreth0@census.gov",
  "gender": "Female",
  "ip_address": "26.58.193.2"
}, {
  "id": 2,
  "first_name": "Giavani",
  "last_name": "Frediani",
  "email": "gfrediani1@senate.gov",
  "gender": "Male",
  "ip_address": "229.179.4.212"
}, {
  "id": 3,
  "first_name": "Noell",
  "last_name": "Bea",
  "email": "nbea2@imageshack.us",
  "gender": "Female",
  "ip_address": "180.66.162.255"
}, {
  "id": 4,
  "first_name": "Willard",
  "last_name": "Valek",
  "email": "wvalek3@vk.com",
  "gender": "Male",
  "ip_address": "67.76.188.26"
}]
*/

i32 main()
{
	basicInfo test;
		test.first_name = "Jeanette";
		test.last_name = "Penddreth";
		test.gender = "jpenddreth0@census.gov";
		test.member1 = 0xBB;
		test.member2 = 0xB00B;
		test.member3 = FLN_SWAP_32(0xB00B1E55);
		test.member4 = 5050;
		test.member5 = 42069;
		test.member6 = 8000;
		test.member7 = 3000;
		test.member8 = 32;
		test.member9 = "Hello World!";

	auto buf = he::BinaDescriptor::StaticBuild<basicInfo>(
		test,
		NULL, 0,
		'L');
	FILE* file = fopen("./stdout.bin", "wb");
	if(!file) return 69;
	fwrite(buf.first.get(), 1, buf.second, file);
    fclose(file);

	return 0;
}
