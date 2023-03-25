#include "HE/HE2/Gedit.hpp"
#include <stdexcept>

namespace fln::he2
{
	GeditDescriptor GeditDescriptor::Load(cstr filePath)
	{
		GeditDescriptor output;
		auto bina = BinaDescriptor::Load(filePath);

		output.m_Bina = bina;
		output.m_Header = (GeditHeader*)bina.m_Nodes[0].m_Data;
		output.m_ObjectOffsetTable = (u64*)(((u8*)output.m_Header) + sizeof(GeditHeader));
		output.m_Objects = (Object*)(output.m_ObjectOffsetTable + output.m_Header->ObjectCount);

		return output;
	}
}
