#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" 
#include "spdlog/sinks/ostream_sink.h "
#include "spdlog/sinks/stdout_color_sinks.h"

namespace shade
{
	class SHADE_API Log
	{
	public:
		// Initializing Log, if arguments were passed then use GeCore or Cleint stream
		static void Init(const bool& cCore = false, const bool& cClient = false);
		inline static shade::Shared<spdlog::logger>& CoreLogger() { return m_sCoreLogger; }
		inline static shade::Shared<spdlog::logger>& ClientLogger() { return m_sClientLogger; }
		inline static std::ostringstream& GetCoreStream() { return m_sCoreStream; }
		inline static std::ostringstream& GetClientStream() { return m_sClientStream; }
	private:
		static shade::Shared<spdlog::logger> m_sCoreLogger;
		static shade::Shared<spdlog::logger> m_sClientLogger;
		static std::ostringstream m_sCoreStream;
		static std::ostringstream m_sClientStream;
	};

#define SHADE_CORE_ERROR(...)     {::shade::Log::CoreLogger()->error(__VA_ARGS__);  abort(); }
#define SHADE_CORE_WARNING(...)   ::shade::Log::CoreLogger()->warn(__VA_ARGS__);
#define SHADE_CORE_INFO(...)      ::shade::Log::CoreLogger()->info(__VA_ARGS__);
#define SHADE_CORE_TRACE(...)     ::shade::Log::CoreLogger()->trace(__VA_ARGS__);
#define SHADE_CORE_DEBUG(...)     ::shade::Log::CoreLogger()->debug(__VA_ARGS__);

#define SHADE_ERROR(...)		  ::shade::Log::ClientLogger()->error(__VA_ARGS__);
#define SHADE_WARNING(...)		  ::shade::Log::ClientLogger()->warn(__VA_ARGS__);
#define SHADE_INFO(...)			  ::shade::Log::ClientLogger()->info(__VA_ARGS__);
#define SHADE_TRACE(...)		  ::shade::Log::ClientLogger()->trace(__VA_ARGS__);
}