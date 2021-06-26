#pragma once
#include "shade/config/API.h"
#include "shade/core/render/RenderContext.h"

struct GLFWwindow;

namespace shade
{
	class OpenGLContext : public RenderContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}