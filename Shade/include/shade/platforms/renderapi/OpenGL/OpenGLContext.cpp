#include "shade_pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

shade::OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	if (!windowHandle)
		SHADE_CORE_ERROR("Window handle is nullptr!");
}

void shade::OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	GLenum status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!status)
	{
		SHADE_CORE_ERROR("Failed to initialize Glad!");
	}

	SHADE_CORE_INFO("OpenGL Info:");
	SHADE_CORE_INFO("Vendor: {0}",   glGetString(GL_VENDOR));
	SHADE_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
	SHADE_CORE_INFO("Version: {0}",  glGetString(GL_VERSION));

	SHADE_INFO("OpenGL Info:");
	SHADE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
	SHADE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
	SHADE_INFO("Version: {0}", glGetString(GL_VERSION));
	/*if (!(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)))
		SHADE_CORE_ERROR("Unuported version OpenGL")*/
}

void shade::OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}
