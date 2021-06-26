#include "shade_pch.h"
#include "RenderContext.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLContext.h"
#include <GLFW/glfw3.h>

shade::Unique<shade::RenderContext> shade::RenderContext::Create(void* window)
{
	return CreateUnique<OpenGLContext>(static_cast<GLFWwindow*>(window));
}
