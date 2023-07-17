#pragma once
#include "pch.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace dvl
{
	class Log
	{
	public:
		inline static std::shared_ptr<spdlog::logger> Logger() { return s_Logger; }

		inline static void PrintBuffer(u8* ptr, u64 len, cstr indent)
		{
			if (len <= UINT32_MAX)
			{
				std::cout << std::hex << std::uppercase <<
					indent << "---------|------------------------------------------------\n" <<
					indent << "         | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
					indent << "---------|------------------------------------------------\n" <<
					indent << "00000000 | ";
				for (u64 i = 0; i < len; ++i)
				{
					std::cout <<
						std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';
					if ((i + 1) % 16 == 0 && i != len - 1)
						std::cout << "\n" << indent <<
						std::setw(8) << std::setfill('0') << i + 1 << " | ";
				}
			}
			else
			{
				std::cout << std::hex << std::uppercase <<
					indent << "-----------------|------------------------------------------------\n" <<
					indent << "                 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" <<
					indent << "-----------------|------------------------------------------------\n" <<
					indent << "0000000000000000 | ";
				for (u64 i = 0; i < len; ++i)
				{
					std::cout <<
						std::setw(2) << std::setfill('0') << (u32)ptr[i] << ' ';
					if ((i + 1) % 16 == 0)
						std::cout << "\n" << indent <<
						std::setw(16) << std::setfill('0') << i + 1 << " | ";
				}
			}
			std::cout << std::dec << std::nouppercase << "\n";
		}

	private:
		inline static std::shared_ptr<spdlog::logger> Init()
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("devil.log", true));

			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			auto out = std::make_shared<spdlog::logger>("DEVIL", begin(logSinks), end(logSinks));
			spdlog::register_logger(out);
			out->set_level(spdlog::level::trace);
			out->flush_on(spdlog::level::trace);

			return out;
		}

		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}
