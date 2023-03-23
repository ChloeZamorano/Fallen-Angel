#include "HE/Bina.hpp"

namespace fln::he
{
	Result<cstr, BinaDescriptor> BinaDescriptor::Load(cstr filePath)
	{
		BinaDescriptor output;

		//
		// Open and read file
		//
		FILE *file = fopen(filePath, "rb");
		if(file == null)
		{
			return
			{
				false,
				"Error opening the file.\n"
				"Does the file exist? Is the path correct?"
				"Does it have read permissions?\n",
				null
			};
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
				ptr = (BinaNode*)(((void*)output.m_Header) + sizeof(BinaHeader));
			else
				ptr = (BinaNode*)(((void*)output.m_Nodes[i-1].m_Node) + output.m_Nodes[i-1].m_Node->Basic.Length);
			output.m_Nodes.push_back(BinaDataNodeDescriptor::Read(ptr).Expect());
		}

		return
		{
			true,
			null,
			output
		};
	}
}