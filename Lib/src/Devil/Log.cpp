#include "pch.hpp"
#include "Devil/Log.hpp"

namespace dvl
{
	std::shared_ptr<spdlog::logger> Log::s_Logger = Log::Init();
}
