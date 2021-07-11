#include "shade_pch.h"
#include "UniformBuffer.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLUniformBuffer.h"

shade::Shared<shade::UniformBuffer> shade::UniformBuffer::Create(const std::uint32_t& size, const std::uint32_t& bindng)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLUniformBuffer>(size, bindng);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
    return Shared<UniformBuffer>();
}
