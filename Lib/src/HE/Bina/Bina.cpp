#include "pch.hpp"

#define REFLECTION_GUARD
	#include "HE/Bina/Bina.hpp"
#undef REFLECTION_GUARD

namespace fln::he
{
	std::unique_ptr<BinaNodeDescriptor> BinaNodeDescriptor::New(u8* base)
	{
		auto output = std::make_unique<BinaNodeDescriptor>();

		output->m_Head = (BinaNode*)base;
		output->m_AdditionalData = base + sizeof(BinaNode);
		output->m_Data = output->m_AdditionalData + output->m_Head->AdditionalDataLength;
		output->m_StringsTable = (cstr)output->m_Data + output->m_Head->StringTableOffset;
		output->m_OffsetsTable = (u8*)output->m_StringsTable + output->m_Head->StringTableLength;

		output->m_DataLength = (u64)output->m_StringsTable - (u64)output->m_Data;

		return output;
	}

	Array<size_t, u32> BinaNodeDescriptor::GetOffsets()
	{
		return UnpackOffsets(m_OffsetsTable, m_Head->OffsetTableLength);
	}

	Array<u8*, u32> BinaNodeDescriptor::GetPointers()
	{
		Array<size_t, u32> offsets = GetOffsets();
		Array<u8*, u32> pointers(offsets.Count());

		pointers[0] = offsets[0] + m_Data;
		for(u32 i = 1; i < offsets.Count(); ++i)
			pointers[i] = pointers[i-1] + offsets[i];

		return pointers;
	}

	Array<cstr, u32> BinaNodeDescriptor::GetStrings()
	{
		std::vector<cstr> output;

		u32 pos = 0;
		while (pos < m_Head->StringTableLength)
		{
			if (m_StringsTable[pos] == NULL)
				break;

			output.push_back(m_StringsTable + pos);
			pos += strlen(m_StringsTable + pos) + 1;
		}
		return Array<cstr, u32>(&output[0], output.size());
	}

	Array<VarBuf, u32> BinaNodeDescriptor::GetMembers()
	{
		Array<u8*, u32> pointers = GetPointers();
		Array<VarBuf, u32> members(pointers.Count());

		for(u32 i = 0; i < members.Count() - 1; ++i)
		{
			members[i].buffer = pointers[i];
			members[i].width = pointers[i+1] - pointers[i];
		}
		members.Last()->buffer = reinterpret_cast<u8*>(pointers.Last());
		members.Last()->width = reinterpret_cast<u8*>(const_cast<char*>(m_StringsTable)) - *pointers.Last();

		return members;
	}

	BinaDescriptor BinaDescriptor::Read(std::unique_ptr<u8[]>&& buf, u64 width)
	{
		BinaDescriptor output;
		output.m_FileSize = width;
		output.m_FileStart = std::move(buf);

		output.m_Header = (BinaHeader*)output.m_FileStart.get();

		if(output.m_Header->NodeCount == 0)
			output.m_Node = NULL;
		else
			output.m_Node = BinaNodeDescriptor::New(output.m_FileStart.get() + sizeof(BinaHeader));

		output.m_VirtualFileEnd = output.m_FileStart.get() + output.m_Header->FileSize;

		output.m_FileEnd = output.m_FileStart.get() + output.m_FileSize;

		return output;
	}

	BinaDescriptor BinaDescriptor::Load(cstr filePath)
	{
		FILE* file = fopen(filePath, "rb");
		fseek(file, 0, SEEK_END);
		u64 width = ftell(file);
		fseek(file, 0, SEEK_SET);

		auto buf = std::make_unique<u8[]>(width);
		fread(buf.get(), width, 1, file);
		fclose(file);
		
		return Read(std::move(buf), width);
	}

	std::pair<std::unique_ptr<u8[]>, u64> BinaDescriptor::Build(
			const Array<VarBuf, u32>& Variables,
			const Array<cstr, u32>& Strings,
			const u8* Secret, const u64 SecretWidth,
			const char Endian)
	{
		//
		// Gather the data from Variables.
		// TODO: Rearrange this to be more readable.
		//
		Array<u32, u32> dataDiffs(Variables.Count());
		u32 dataSize = HE_BINA_FIRST_OFFSET;
		for(u32 i = 0; i < Variables.Count(); ++i)
		{
			if (Variables[i].width >= 4 && (Variables[i].width & 3) == 0)
				dataDiffs[i] = 0;
			else
			{
				u32 remainder = Variables[i].width % 4;
            	if (remainder == 0)
            	    dataDiffs[i] = 4;
				else
            	    dataDiffs[i] = 4 - remainder;
			}
			dataSize += Variables[i].width + dataDiffs[i];
		}

		Array<u8, u32> data(dataSize);
		u8* pos = reinterpret_cast<u8*>(data.Begin() + HE_BINA_FIRST_OFFSET);
		for(u32 i = 0; i < Variables.Count(); ++i)
		{
			u8* var = Variables[i].buffer;
			memcpy(pos, var, Variables[i].width);
			
			pos += Variables[i].width + dataDiffs[i];
		}

		//
		// Chain the strings into an string table
		//
		u32 strSize = 0;
		for(u32 i = 0; i < Strings.Count(); ++i)
			strSize += strlen(Strings[i]) + 1;

		Array<u8, u32> strings(strSize + 3);
		memset(strings.Begin(), 0, strings.Size());	
		pos = strings.Begin();
		for(u32 i = 0; i < Strings.Count(); ++i)
		{
			memcpy(pos, Strings[i], strlen(Strings[i]));
			pos += strlen(Strings[i]) + 1;
		}

		//
		// Convert the variable widths to valid offsets and pack them into the format Bina uses
		//
		Array<u8, u32> offsets = PackOffsets(Variables, dataDiffs);

		//
		// Setup the header and the node.
		//
		auto header = BinaHeader{};
		header.EndianFlag = Endian;
		header.FileSize =
			sizeof(BinaHeader) +
			sizeof(BinaNode) +
			24 +
			data.Size() +
			strings.Size() +
			offsets.Size();
		header.NodeCount = 1;

		auto node = BinaNode{};
		node.Length =
			header.FileSize - 16;
		node.StringTableOffset = data.Size();
		node.StringTableLength = strings.Size();
		node.OffsetTableLength = offsets.Size();


		FUCK_TIM_COOK();

		//
		// Assemble the file
		//
		auto fileBuf = std::make_unique<u8[]>
			(header.FileSize + SecretWidth);
		u8* filePtr = fileBuf.get();

		memcpy(filePtr, &header, sizeof(BinaHeader));
		filePtr += sizeof(BinaHeader);

		memcpy(filePtr, &node, sizeof(BinaNode));
		filePtr += sizeof(BinaNode);

		memset(filePtr, 0, node.AdditionalDataLength);
		filePtr += node.AdditionalDataLength;

		memcpy(filePtr, data.Begin(), data.Size());
		filePtr += data.Size();

		memcpy(filePtr, strings.Begin(), strings.Size());
		filePtr += strings.Size();

		memcpy(filePtr, offsets.Begin(), offsets.Size());
		filePtr += offsets.Size();

		memcpy(filePtr, Secret, SecretWidth);

		return std::make_pair<std::unique_ptr<u8[]>, u64>
			(std::move(fileBuf), header.FileSize + SecretWidth);
	}
}