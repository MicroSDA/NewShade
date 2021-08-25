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
	case GL_DEBUG_SEVERITY_HIGH:         SHADE_CORE_WARNING(message);		return;
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
}

void shade::OpenGLRenderAPI::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	glClearColor(r, g, b, a);
}

void shade::OpenGLRenderAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void shade::OpenGLRenderAPI::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	glViewport(x, y, width, height);
}

void shade::OpenGLRenderAPI::BeginScene(const Shared<Shader>& shader, const Shared<Camera>& camera, const Shared<Environment>* enviroments, const std::size_t& enviromentsCount)
{
	if (enviroments != nullptr && enviromentsCount > 0)
	{
		for (auto i = 0; i < enviromentsCount; i++)
		{
			enviroments[i]->Process(shader);
		}
	}
}

void shade::OpenGLRenderAPI::EndScene(const Shared<Shader>& shader)
{
	shader->UnBind();
}

void shade::OpenGLRenderAPI::DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO) const
{

	VAO->Bind(); glDrawElements(static_cast<GLenum>(mode), IBO->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void shade::OpenGLRenderAPI::DrawInstanced(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO, const std::uint32_t& instanceCount)
{
	VAO->Bind(); glDrawElementsInstanced(static_cast<GLenum>(mode), IBO->GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
}

void shade::OpenGLRenderAPI::DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)
{
	VAO->Bind();  glDrawArrays(static_cast<GLenum>(mode), 0, count);
}

void shade::OpenGLRenderAPI::Begin(Shared<FrameBuffer> framebuffer)
{
	if (framebuffer != nullptr)
	{
		framebuffer->Bind();
		Clear();
	}
}

void shade::OpenGLRenderAPI::End(Shared<FrameBuffer> framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shade::OpenGLRenderAPI::DepthTest(const bool& enable)
{
	(enable) ? glEnable(GL_DEPTH_TEST): glDisable(GL_DEPTH_TEST);
}
