#include "shade_pch.h"
#include "VertexArray.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLVertexArray.h"


shade::Shared<shade::VertexArray> shade::VertexArray::Create()
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL:	return CreateShared<OpenGLVertexArray>();
	case RenderAPI::API::Vulkan:
		SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}
