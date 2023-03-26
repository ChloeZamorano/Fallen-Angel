#include "HE/HE2/Gedit.hpp"

namespace fln::he2
{
	Result<GeditDescriptor> GeditDescriptor::Load(cstr filePath)
	{
		GeditDescriptor output;
		auto binaR = BinaDescriptor::Load(filePath);
		if (!binaR.m_IsOk)
			return Result<GeditDescriptor>
			{
				.m_IsOk = false,
				.m_Error = binaR.m_Error,
				.m_Ok = output
			};
		output.m_Bina = binaR.m_Ok;

		auto node = std::static_pointer_cast<he::BinaDataNodeDescriptor>(output.m_Bina.m_Nodes[0]);

		return Result<GeditDescriptor>
		{
			.m_IsOk = true,
			.m_Error = "",
			.m_Ok = output
		};
	}
}
