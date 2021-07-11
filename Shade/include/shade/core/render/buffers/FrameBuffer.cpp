#include "shade_pch.h"
#include "FrameBuffer.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLFrameBuffer.h"

shade::Shared<shade::FrameBuffer> shade::FrameBuffer::Create(const Layout& layout)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::OpenGL:	return CreateShared<OpenGLFrameBuffer>(layout);
	case RenderAPI::API::Vulkan:
		SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}