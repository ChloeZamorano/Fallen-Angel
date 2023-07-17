#include "pch.hpp"
#include "HE/Bina/Bina.hpp"

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

	u64 BinaNodeDescriptor::GetNextOffset(u32& i)
	{
		auto ptr = (u8*)(m_OffsetsTable + i);
		u32 b1 = (u32)(ptr[0] & OFFSET_06_DATA_MASK);
		u32 b2 = (u32)(ptr[1]);
		u32 b3 = (u32)(ptr[2]);
		u32 b4 = (u32)(ptr[3]);
		u64 offset = 0;
		switch (*(OffsetMask*)ptr & OffsetMask::OFFSET_SIZE_MASK)
		{
		case OffsetMask::OFFSET_SIZE_06:
			offset = (u64)b1 << 2;
			++i;
			return offset;
		case OffsetMask::OFFSET_SIZE_14:
			offset = (u64)(
				((b1 << 8) | b2)
				<< 2);
			i += 2;
			return offset;
		case OffsetMask::OFFSET_SIZE_30:
			offset = (u64)(
				((b1 << 24) | (b2 << 16) | (b3 << 8) | b4)
				<< 2);
			i += 4;
			return offset;
		default:
			return NULL;
		}
	}

	u64 BinaNodeDescriptor::GetNextFinalOffset(u64 previousPtr, u32& i)
	{
		std::cout << "";
		auto ptr = (u8*)(m_OffsetsTable + i);
		u32 b1 = (u32)(ptr[0] & OFFSET_06_DATA_MASK);
		u32 b2 = (u32)(ptr[1]);
		u32 b3 = (u32)(ptr[2]);
		u32 b4 = (u32)(ptr[3]);
		u64 offset = 0;
		switch (*(OffsetMask*)ptr & OffsetMask::OFFSET_SIZE_MASK)
		{
		case OffsetMask::OFFSET_SIZE_06:
			offset = (u64)b1 << 2;
			++i;
			return previousPtr + offset;
		case OffsetMask::OFFSET_SIZE_14:
			offset = (u64)(
				((b1 << 8) | b2)
				<< 2);
			i += 2;
			return previousPtr + offset;
		case OffsetMask::OFFSET_SIZE_30:
			offset = (u64)(
				((b1 << 24) | (b2 << 16) | (b3 << 8) | b4)
				<< 2);
			i += 4;
			return previousPtr + offset;
		default:
			return NULL;
		}
	}

	std::vector<u64> BinaNodeDescriptor::GetOffsets()
	{
		std::vector<u64> output;
		u32 i = 0;
		while (i < m_Head->OffsetTableLength)
		{
			u64 ptr = GetNextOffset(i);
			if (ptr == NULL)
				return output;
			output.emplace_back(ptr);
		}
		return output; // This should never happen.
	}

	std::vector<u64> BinaNodeDescriptor::GetFinalOffsets()
	{
		std::vector<u64> output;
		u32 i = 0;
		while(i < m_Head->OffsetTableLength)
		{
			u64 prevptr = output.size() == 0 ?
				64 :
				output[output.size()-1];

			u64 ptr = GetNextFinalOffset(prevptr, i);
			if (ptr == NULL)
				return output;
			output.emplace_back(ptr);
		}
		return output; // This should never happen.
	}

	u8* BinaNodeDescriptor::GetNextPointer(u8* previousPtr, u32& i)
	{
		return (u8*)GetNextFinalOffset((u64)previousPtr, i);
	}

	std::vector<u8*> BinaNodeDescriptor::GetPointers(u8* header)
	{
		std::vector<u8*> output;
		u32 i = 0;
		while(i < m_Head->OffsetTableLength)
		{
			u8* prevptr = output.size() == 0 ?
				header + 64 :
				output[output.size()-1];

			u8* ptr = GetNextPointer(prevptr, i);
			if (ptr == NULL)
				return output;
			output.push_back(ptr);
		}
		return output; // This should never happen.
	}

	std::vector<cstr> BinaNodeDescriptor::GetStrings()
	{
		std::vector<cstr> output;

		u32 pos = 0;
		while (pos < m_Head->StringTableLength)
		{
			if (m_StringsTable[pos] == NULL)
				break;

			output.emplace_back(m_StringsTable + pos);
			pos += strlen(m_StringsTable + pos) + 1;
		}
		return output;
	}

	// TODO: Something's wrong with the last size, and possibly the pointers too
	std::vector<std::pair<u8*, u64>> BinaNodeDescriptor::GuessMembers(u8* header)
	{
		std::vector<std::pair<u8*, u64>> output;
		std::vector<u64> offs = GetOffsets();
		std::vector<u64> foffs = GetFinalOffsets();
		std::vector<u8*> ptrs = GetPointers(header);

		output.emplace_back(std::make_pair(m_Data, offs[1]));

		for (u32 i = 1; i < (offs.size() - 1); ++i)
			output.emplace_back(std::make_pair(ptrs[i], offs[i + 1]));

		output.emplace_back(std::make_pair(
			ptrs[ptrs.size() - 1],
			(u64)m_StringsTable - (u64)ptrs[ptrs.size() - 1]));

		return output;
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
		DVL_ASSERT(ERROR, file != NULL, "{0} {1}: Could not read file {2}.",
			__FILENAME__, __LINE__, filePath);
		fseek(file, 0, SEEK_END);
		u64 width = ftell(file);
		fseek(file, 0, SEEK_SET);

		auto buf = std::make_unique<u8[]>(width);
		fread(buf.get(), width, 1, file);
		fclose(file);
		
		return Read(std::move(buf), width);
	}

	std::unique_ptr<u8[]> BinaDescriptor::Build(
			Array<VarBuf, u32> Variables,
			Array<cstr, u32> Strings,
			u8* XData, u64 XDataLen)
	{
		std::vector<u8> data;
		std::vector<u8, u32> strings;

		//
		// Gather the data from Variables.
		//
		for(u32 i = 0; i < Variables.Count(); ++i)
		{
			u8* ptr = (u8*)&Variables[i].buffer;

			for(u32 j = 0; j < Variables[i].width; ++j)
				data.emplace_back(ptr[j]);

			if ((Variables[i].width & 0x3) != 0)
			{
				// I dunno if this will work man, I didn't want to think much about it so I had
				// ChatGipity solve this one for me, this here is supposed to calculate how much
				// I have to add to the number to make the last two bits zero, so that no data
				// will be lost when it gets bit shifted to go into the offsets table.
				u32 diff = (4 - (Variables[i].width & 3)) & 3;
        		Variables[i].width += diff;

				for(u32 j = 0; j < diff; ++j)
					data.emplace_back(0);
    		}
		}

		//
		// Chain the strings into an string table
		//
		for(u32 i = 0; i < Strings.Count(); ++i)
		{
			u32 len = strlen(Strings[i]);
			for(u32 j = 0; j < len; ++j)
			{
				strings.emplace_back(Strings[i][j]);
				strings.emplace_back(0);
			}
		}
		strings.emplace_back(0);
		strings.emplace_back(0);
		strings.emplace_back(0);
		strings.emplace_back(0);

		//
		// Convert the variable widths to valid offsets and pack them into the format Bina uses
		//
		Array<u8, u32> offsets = PackOffsets(Variables);

		//
		// Setup the header and the node.
		//
		auto node = (BinaNode)0;
		node.Length =
			sizeof(BinaNode) +
			24 +
			data.size() +
			strings.size() +
			offsets.Size();
		node.StringTableOffset = data.size();
		node.StringTableLength = strings.size();
		node.OffsetTableLength = offsets.Size();

		auto header = (BinaHeader)0;
		header.FileSize =
			sizeof(BinaHeader) +
			node.Length;
		header.NodeCount = 1;

		//
		// Assemble the file
		//
		auto fileBuf = std::make_unique<u8[]>
			(header.FileSize + XDataLen);
		u8* filePtr = fileBuf.get();

		memcpy(filePtr, &header, sizeof(BinaHeader));
		filePtr += sizeof(BinaHeader);

		memcpy(filePtr, &node, sizeof(BinaNode));
		filePtr += sizeof(filePtr);

		memset(filePtr, 0, node.AdditionalDataLength);
		filePtr += node.AdditionalDataLength;

		memcpy(filePtr, &data[0], data.size());
		filePtr += data.size();

		memcpy(filePtr, &strings[0], strings.size());
		filePtr += strings.size();

		memcpy(filePtr, offsets.Begin(), offsets.Size());
		filePtr += offsets.Size();

		memcpy(filePtr, XData, XDataLen);

		return fileBuf;
	}
}