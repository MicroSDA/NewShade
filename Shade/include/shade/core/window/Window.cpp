#include "shade_pch.h"
#include "Window.h"

#ifdef SHADE_WINDOWS_PLATFORM
	#include "shade/platforms/window/WindowsWindow.h"
#endif

namespace shade
{
	Unique<Window> Window::Create(const WindowConfig& config)
	{
#ifdef SHADE_WINDOWS_PLATFORM
		return CreateUnique<WindowsWindow>(config);
#else
		SHADE_CORE_ERROR("Undefined window's platform!")
		return nullptr;
#endif
	}
}