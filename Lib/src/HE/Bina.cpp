#include "HE/Bina.hpp"
#include <string>

namespace fln::he
{
	Result<BinaDescriptor> BinaDescriptor::Load(cstr filePath)
	{
		BinaDescriptor output;

		//
		// Open and read file
		//
		FILE* file;
		errno_t err;
		if ((err = fopen_s(&file, filePath, "rb")) != NULL)
		{
			std::stringstream ss;
			ss << "Could not read file \"" << filePath << "\"\n" <<
				"Error code: " << err;
			return Result<BinaDescriptor>
			{
				.m_IsOk = false,
				.m_Error = ss.str(),
				.m_Ok = output
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
				ptr = (BinaNode*)(((u8*)output.m_Header) + sizeof(BinaHeader));
			else
				ptr = (BinaNode*)(((u8*)output.m_Nodes[i-1]->m_Head) + output.m_Nodes[i-1]->m_Head->Length);
			output.m_Nodes.emplace_back(BinaNodeDescriptor::New(ptr));
		}
		return Result<BinaDescriptor>
		{
			.m_IsOk = true,
			.m_Error = "",
			.m_Ok = output
		};
	}
}