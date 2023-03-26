#pragma once
#include "pch.hpp"

namespace dvl
{
	template<typename K>
	class Result
	{
	public:
		bool m_IsOk;
		std::string m_Error;
		K m_Ok;

		inline K Expect()
		{
			if (m_IsOk)
				return m_Ok;
			throw std::logic_error(m_Error);
		}
	};
}
