#include "shade_pch.h"
#include "IndexBuffer.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLIndexBuffer.h"

shade::Shared<shade::IndexBuffer> shade::IndexBuffer::Create(Index* indices, std::uint32_t count)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLIndexBuffer>(indices, count);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}
