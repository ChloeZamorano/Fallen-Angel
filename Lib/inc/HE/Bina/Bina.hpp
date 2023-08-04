#pragma once
#include "pch.hpp"

#include "Devil/Array.inl"
#include "HE/OffsetPack.hpp"

#ifndef REFLECTION_GUARD
	#define X(TYPE) TYPE,
	#define RFL_TYPE_ENTRIES()	\
		X(u8)					\
		X(u16)					\
		X(u32)					\
		X(u64)					\
		X(i8)					\
		X(i16)					\
		X(i32)					\
		X(i64)					\
		X(f32)					\
		X(f64)					\
		X(cstr)					\

	#include "Devil/Reflection/Reflect.inl"
#endif

namespace fln::he
{
	struct BinaNode
	{
		const char Signature[4] = {'D','A','T','A'};
		u32 Length;

		u32 StringTableOffset;
		u32 StringTableLength;

		u32 OffsetTableLength;
		const u16 AdditionalDataLength = 24;
		const u16 Padding = 0;
	};

	class BinaNodeDescriptor
	{
	public:
		BinaNode* m_Head;

		// Exact purpose unknown. Seems to always just contain nulls?
		u8* m_AdditionalData;
		// Specific to each type of file. E.G. gedit files contain gedit data.
		u8* m_Data;
		cstr m_StringsTable;
		u8* m_OffsetsTable;

		u64 m_DataLength;

		static std::unique_ptr<BinaNodeDescriptor> New(u8* base);

		Array<size_t, u32> GetOffsets();
		Array<u8*, u32> GetPointers();

		Array<cstr, u32> GetStrings();

		Array<VarBuf, u32> GetMembers();
	};

	struct BinaHeader
	{
		const char Signature[4] = {'B','I','N','A'};
		const char Version[3] = {'2','1','0'};
		char EndianFlag = 'L';
		u32 FileSize;
		u16 NodeCount;
		const u16 Unknown = 0;	// TODO: Look into what's up with this value
	};

	// TODO: Handle endianness
	class BinaDescriptor
	{
	public:
		std::unique_ptr<u8[]> m_FileStart;
			BinaHeader* m_Header;
			std::unique_ptr<BinaNodeDescriptor> m_Node;
			u8* m_VirtualFileEnd;
		u8* m_FileEnd;
		
		u64 m_FileSize;
		u64 m_SecretWidth;

		
		static BinaDescriptor Read(std::unique_ptr<u8[]>&& buf, u64 width);

		static BinaDescriptor Load(cstr filePath);

		static std::pair<std::unique_ptr<u8[]>, u64> Build(
			const Array<VarBuf, u32>& Variables,
			const Array<cstr, u32>& Strings,
			const u8* Secret, const u64 SecretWidth,
			const char Endian);

#ifndef REFLECTION_GUARD
		template<typename T>
		static std::pair<std::unique_ptr<u8[]>, u64> StaticBuild(
			const T& object,
			const u8* Secret, const u64 SecretWidth,
			const char Endian)
		{
			const Class* objInf = GetClass<T>();
			std::vector<VarBuf> Variables;
			std::vector<cstr> Strings;

			for (const auto& field : objInf->Fields)
			{
				const u8* source = reinterpret_cast<const u8*>(&object) + field.Offset;

				switch (field.TypeInfo->ID)
				{
					case TypeName::i8:
					case TypeName::i16:
					case TypeName::i32:
					case TypeName::u8:
					case TypeName::u16:
					case TypeName::u32:
					{
						Variables.emplace_back(VarBuf{
							.buffer = const_cast<u8*>(source),
							.width = field.TypeInfo->Size
						});
						break;
					}
					case TypeName::cstr:
					{
						Strings.emplace_back(reinterpret_cast<cstr*>(const_cast<u8*>(source))[0]);
					}
					default:
						
						break;
				}
			}

			return Build(
				Array<VarBuf, u32>(&Variables[0], Variables.size()),
				Array<cstr, u32>(&Strings[0], Strings.size()),
				Secret, SecretWidth,
				Endian
			);
		}
#endif
	};
}
