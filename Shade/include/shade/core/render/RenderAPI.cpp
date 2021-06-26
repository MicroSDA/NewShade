#include "shade_pch.h"
#include "RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLRenderAPI.h"

shade::RenderAPI::API shade::RenderAPI::m_sRenderAPI = shade::RenderAPI::API::OpenGL;

shade::RenderAPI::API shade::RenderAPI::GetAPI()
{
	return m_sRenderAPI;
}

shade::Unique<shade::RenderAPI>shade::RenderAPI::Create()
{
	switch (m_sRenderAPI)
	{
	case API::None:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case API::OpenGL:	return CreateUnique<OpenGLRenderAPI>();
	case API::Vulkan:
		SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}	
}
