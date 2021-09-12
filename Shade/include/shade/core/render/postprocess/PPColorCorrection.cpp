#include "shade_pch.h"
#include "PPColorCorrection.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/postprocess/OpenGL_PColorCorrection.h"

shade::Shared<shade::PPColorCorrection> shade::PPColorCorrection::Create()
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGL_PColorCorrection>();
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

shade::PPColorCorrection::PPColorCorrection() :
	m_Gamma(1.f), m_Exposure(1.f), m_ToneMapping(ToneMapping::ACES)
{
}

void shade::PPColorCorrection::SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader)
{
	m_InputFrameBuffer = input;
	m_OutputFrameBuffer = output;
	m_Shader = shader;
}

const float& shade::PPColorCorrection::GetExposure() const
{
	return m_Exposure;
}

const float& shade::PPColorCorrection::GetGamma() const
{
	return m_Gamma;
}

float& shade::PPColorCorrection::GetExposure()
{
	return const_cast<float&>(const_cast<const PPColorCorrection*>(this)->GetExposure());
}

float& shade::PPColorCorrection::GetGamma()
{
	return const_cast<float&>(const_cast<const PPColorCorrection*>(this)->GetGamma());
}
