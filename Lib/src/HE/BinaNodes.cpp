#include "HE/BinaNodes.hpp"

namespace fln::he
{
	Result<cstr, BinaDataNodeDescriptor>
		BinaDataNodeDescriptor::Read(BinaNode* basicNode)
	{
		BinaDataNodeDescriptor output;
		output.m_Node = (BinaDataNode*)basicNode;
		output.m_AdditionalData = (((u8*)basicNode) + sizeof(BinaDataNode));
		output.m_Data = output.m_AdditionalData + output.m_Node->AdditionalDataLength;
		output.m_StringsTable = (cstr)output.m_Data + output.m_Node->StringTableOffset;
		output.m_OffsetsTable = (u8*)output.m_StringsTable + output.m_Node->StringTableLength;

		output.m_DataLength =
			output.m_Node->Basic.Length - (sizeof(BinaDataNode)
			+ output.m_Node->AdditionalDataLength);

		return
		{
			true,
			null,
			output
		};
	}
}
