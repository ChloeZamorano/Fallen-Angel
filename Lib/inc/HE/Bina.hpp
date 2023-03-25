#pragma once
#include "pch.hpp"

#include "HE/BinaNodes.hpp"
#include "Result.tpp"

namespace fln::he
{
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
		std::vector<BinaDataNodeDescriptor> m_Nodes;
		void* m_EndOfVirtualFile;

		// This is useful to leave data at the end of the file that the game cannot see,
		// or more accurately won't look at.
		u64 m_PhysicalFileSize;

		~BinaDescriptor();
		static BinaDescriptor Load(cstr filePath);
	};
}
