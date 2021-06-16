#include "shade_pch.h"
#include "Timer.h"
#include <GLFW/glfw3.h>

shade::Timer::Timer():
	m_TimeNow(glfwGetTime()), m_TimeLast(0.0f), m_DeltaTime(0.0f)
{
}

void shade::Timer::Update()
{
	m_TimeLast = m_TimeNow;
	m_TimeNow = static_cast<float>(glfwGetTime());
	m_DeltaTime = m_TimeNow - m_TimeLast;
}
