#include "shade_pch.h"
#include "ShaderStorageBuffer.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLShaderStorageBuffer.h"

const std::uint32_t& shade::ShaderStorageBuffer::GetSize() const
{
	return m_Size;
}

shade::Shared<shade::ShaderStorageBuffer> shade::ShaderStorageBuffer::Create(const std::uint32_t& size, const std::uint32_t& bindng)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLShaderStorageBuffer>(size, bindng);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}