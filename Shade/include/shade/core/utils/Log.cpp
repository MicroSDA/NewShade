#include "shade_pch.h"
#include "Log.h"

shade::Shared<spdlog::logger> shade::Log::m_sCoreLogger;
shade::Shared<spdlog::logger> shade::Log::m_sClientLogger;
std::ostringstream			  shade::Log::m_sClientStream;
std::ostringstream			  shade::Log::m_sCoreStream;

void shade::Log::Init(const bool& cCore, const bool& cClient)
{
	spdlog::set_pattern("%^[%T]%n:%v%$");

	if (cCore)
	{
		m_sCoreLogger = std::make_shared<spdlog::logger>("CORE", std::make_shared<spdlog::sinks::ostream_sink_mt>(m_sCoreStream));
		m_sCoreLogger->set_level(spdlog::level::trace);
	}
	else
	{
		m_sCoreLogger = spdlog::stderr_color_mt("CORE");
		m_sCoreLogger->set_level(spdlog::level::trace);
	}

	if (cClient)
	{
		m_sClientLogger = std::make_shared<spdlog::logger>("APPLICATION", std::make_shared<spdlog::sinks::ostream_sink_mt>(m_sClientStream));
		m_sClientLogger->set_level(spdlog::level::trace);
	}
	else
	{
		m_sClientLogger = spdlog::stderr_color_mt("APPLICATION");
		m_sClientLogger->set_level(spdlog::level::trace);

	}
}