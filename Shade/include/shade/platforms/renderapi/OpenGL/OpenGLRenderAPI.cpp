#include "shade_pch.h"
#include "OpenGLRenderAPI.h"

#include <glad/glad.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


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
	//glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);

	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
	glLineWidth(5.f);
}

void shade::OpenGLRenderAPI::ShutDown()
{
}

unsigned int shade::OpenGLRenderAPI::GetVideoMemoryUsage()
{
	//// Nvidia only !
	//GLint nTotalMemoryInKB = 0;
	//glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
	//	&nTotalMemoryInKB);

	//GLint nCurAvailMemoryInKB = 0;
	//glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
	//	&nCurAvailMemoryInKB);

	//return  (nTotalMemoryInKB - nCurAvailMemoryInKB) / 1024;
	return - 1;
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

void shade::OpenGLRenderAPI::BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env, const std::size_t& envCount)
{

}

void shade::OpenGLRenderAPI::EndScene()
{
	
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

void shade::OpenGLRenderAPI::Begin()
{
	Clear();
}

void shade::OpenGLRenderAPI::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shade::OpenGLRenderAPI::DepthTest(const bool& enable)
{
	(enable) ? glEnable(GL_DEPTH_TEST): glDisable(GL_DEPTH_TEST);
}

void shade::OpenGLRenderAPI::CullFace(const int& mode)
{
	glCullFace(mode);
}

void shade::OpenGLRenderAPI::Enable(const int& value)
{
	glEnable(value);
}

void shade::OpenGLRenderAPI::Disable(const int& value)
{
	glDisable(value);
}
