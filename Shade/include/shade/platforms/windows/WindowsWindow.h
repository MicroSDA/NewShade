#pragma once
#include "shade/core/window/Window.h"
#include "shade/core/render/RenderContext.h"
#include <GLFW/glfw3.h>

namespace shade
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowConfig& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override;
		unsigned int GetHeight() const override;
		virtual std::pair<uint32_t, uint32_t> GetSize() const override;

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowConfig& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		Unique<RenderContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool Fullscreen;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}