#include "HE/HE2/Reflection.hpp"

namespace fln::he2
{
	using namespace fln::he;

	Result<Reflection> Reflection::Load(cstr filePath)
	{
		Reflection output;
		auto binaR = BinaDescriptor::Load(filePath);
		if (!binaR.m_IsOk)
			return Result<Reflection>
		{
			.m_IsOk = false,
				.m_Error = binaR.m_Error,
				.m_Ok = output
		};
		output.m_Bina = binaR.m_Ok;

		return Result<Reflection>
		{
			.m_IsOk = true,
				.m_Error = "",
				.m_Ok = output
		};
	}

	std::vector<u8*> Reflection::GetPointers()
	{
		return std::static_pointer_cast<BinaDataNodeDescriptor>
			(m_Bina.m_Nodes[0])->GetPointers((u8*)m_Bina.m_Header);
	}

	std::vector<cstr> Reflection::GetStrings()
	{
		return std::static_pointer_cast<BinaDataNodeDescriptor>
			(m_Bina.m_Nodes[0])->GetStrings();
	}
}
