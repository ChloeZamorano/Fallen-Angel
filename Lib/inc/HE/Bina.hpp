#pragma once
#include "pch.hpp"

#include "HE/BinaNodes.hpp"
#include "Result.tpp"

namespace fln::he
{
	#pragma pack(1)
	struct BinaHeader
	{
		char Signature[4];
		char Version[3];
		u8 EndianFlag;
		u32 FileSize;
		u16 NodeCount;
		u16 Unknown;	// Look into what's up with this value
	};

	class BinaDescriptor
	{
	public:
		BinaHeader* m_Header;
		// This is an array of u8's so that nodes of different sizes can be addressed.
		std::vector<BinaDataNodeDescriptor> m_Nodes;
		void* m_EndOfVirtualFile;

		// This is useful to leave data at the end of the file that the game cannot see,
		// or more accurately won't look at.
		u64 m_PhysicalFileSize;

		static Result<cstr, BinaDescriptor> Load(cstr filePath);
	};
}
