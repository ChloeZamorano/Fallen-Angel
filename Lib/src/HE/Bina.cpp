#include "HE/Bina.hpp"

namespace fln::he
{
	BinaDescriptor::~BinaDescriptor()
	{
		free(m_Header);
	}

	BinaDescriptor BinaDescriptor::Load(cstr filePath)
	{
		BinaDescriptor output;

		//
		// Open and read file
		//
		FILE *file = fopen(filePath, "rb");
		if(file == null)
		{
			throw std::logic_error("");
		}
		fseek(file, 0, SEEK_END);
		output.m_PhysicalFileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		//
		// Setup descriptor and close file
		//
		output.m_Header = (BinaHeader*)malloc(output.m_PhysicalFileSize);
		fread(output.m_Header, output.m_PhysicalFileSize, 1, file);
		fclose(file);

		output.m_EndOfVirtualFile = output.m_Header + output.m_Header->FileSize;

		// Get node pointers
		for(i32 i = 0; i < output.m_Header->NodeCount; ++i)
		{
			BinaNode* ptr;
			if(i == 0)
				ptr = (BinaNode*)(((u8*)output.m_Header) + sizeof(BinaHeader));
			else
				ptr = (BinaNode*)(((u8*)output.m_Nodes[i-1].m_Node) + output.m_Nodes[i-1].m_Node->Basic.Length);
			output.m_Nodes.push_back(BinaDataNodeDescriptor(ptr));
		}

		return output;
	}
}