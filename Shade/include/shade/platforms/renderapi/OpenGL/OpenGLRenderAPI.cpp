#include "shade_pch.h"
#include "OpenGLRenderAPI.h"

#include <glad/glad.h>

void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         SHADE_CORE_ERROR(message);		return;
	case GL_DEBUG_SEVERITY_MEDIUM:       SHADE_CORE_WARNING(message);	return;
	case GL_DEBUG_SEVERITY_LOW:          SHADE_CORE_WARNING(message);	return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: SHADE_CORE_TRACE(message);		return;
	}
}

void shade::OpenGLRenderAPI::Init()
{
#if SHADE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void shade::OpenGLRenderAPI::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	glClearColor(r, g, b, a);
}

void shade::OpenGLRenderAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shade::OpenGLRenderAPI::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	glViewport(x, y, width, height);
}

void shade::OpenGLRenderAPI::DrawIndexed(const Shared<VertexArray>& vao)
{
	vao->Bind();
	glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
