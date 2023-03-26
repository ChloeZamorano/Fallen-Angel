#pragma once
#include "pch.hpp"

#include "HE/Bina.hpp"
#include "Devil/Result.inl"

namespace fln::he2
{
	using namespace fln::he;

	// TODO: Give this an API to handle however this format works.
	// Where's documentation when you need it, am I right? Hah!
	class Reflection
	{
	public:
		BinaDescriptor m_Bina;

		static Result<Reflection> Load(cstr filePath);

		std::vector<u8*> GetPointers();
		std::vector<cstr> GetStrings();
	};
}
