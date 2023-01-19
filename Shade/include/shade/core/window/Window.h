#pragma once
#include "shade/core/events/Event.h"
#include "shade/core/utils/Log.h"

namespace shade
{
	struct WindowConfig
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool Fullscreen;

		WindowConfig(const std::string& title = "Shade",
			uint32_t width = 1600,
			uint32_t height = 900,
			bool fullscreen = false)
			: Title(title), Width(width), Height(height), Fullscreen(fullscreen)
		{
		}
	};

	// Interface representing a desktop system based Window
	class SHADE_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth()  const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;
		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Unique<Window> Create(const WindowConfig& config = WindowConfig());
	};
}