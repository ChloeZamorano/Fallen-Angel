#pragma once
#include "pch.hpp"

#include "HE/BinaNodes.hpp"

namespace fln::he
{
	// TODO: Handle endianness
	struct BinaHeader
	{
		char Signature[4];
		char Version[3];
		u8 EndianFlag;
		u32 FileSize;
		u16 NodeCount;
		u16 Unknown;	// TODO: Look into what's up with this value
	};

	class BinaDescriptor
	{
	public:
		BinaHeader* m_Header;
		// TODO: Use a template or something instead
		// to support different types of nodes.
		std::vector<BinaDataNodeDescriptor> m_Nodes;
		void* m_EndOfVirtualFile;

		// This is useful to leave data at the end of the file that the game cannot see,
		// or more accurately won't look at.
		u64 m_PhysicalFileSize;

		~BinaDescriptor();
		static BinaDescriptor Load(cstr filePath);
	};
}
