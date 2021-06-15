#include "shade_pch.h"
#include "Log.h"

shade::Shared<spdlog::logger> shade::Log::m_sCoreLogger;
shade::Shared<spdlog::logger> shade::Log::m_sClientLogger;

void shade::Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	m_sCoreLogger = spdlog::stderr_color_mt("SHADE");
	m_sCoreLogger->set_level(spdlog::level::trace);

	m_sClientLogger = spdlog::stderr_color_mt("APPLICATION");
	m_sClientLogger->set_level(spdlog::level::trace);
}