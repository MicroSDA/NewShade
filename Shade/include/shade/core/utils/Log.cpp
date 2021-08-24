#include "shade_pch.h"
#include "Log.h"

shade::Shared<spdlog::logger> shade::Log::m_sCoreLogger;
shade::Shared<spdlog::logger> shade::Log::m_sClientLogger;
std::ostringstream			  shade::Log::m_sCoreLoggerStream;

static void Hendler(const std::string& msg)
{
	std::cout << "CustomHenlder\n";
}

void shade::Log::Init()
{
	spdlog::set_pattern("%^[%T]%n:%v%$");

	m_sClientLogger = spdlog::stderr_color_mt("APPLICATION");
	m_sClientLogger->set_level(spdlog::level::trace);

	// Get messages from stream only, done for imgui console
	m_sCoreLogger = std::make_shared<spdlog::logger>("CORE", std::make_shared<spdlog::sinks::ostream_sink_mt>(m_sCoreLoggerStream));
	m_sCoreLogger->set_level(spdlog::level::trace);
}