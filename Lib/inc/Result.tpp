#pragma once
#include "pch.hpp"

template<class E, class K>
class Result
{
public:
	u8 m_IsOk;
	E m_Err;
	K m_Ok;

	K Expect()
	{
		if(m_IsOk)
			return m_Ok;
		throw std::logic_error(m_Err);
	}
};
