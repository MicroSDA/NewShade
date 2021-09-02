#include "shade_pch.h"
#include "PPBloom.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/postprocess/OpenGL_PBloom.h"
#define M_PI           3.14159265358979323846  /* pi */

shade::Shared<shade::PPBloom> shade::PPBloom::Create()
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGL_PBloom>();
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

shade::PPBloom::PPBloom()
{
	m_GaussianUniformBuffer = UniformBuffer::Create(sizeof(PPBloom::ShaderData), 2);
}

void shade::PPBloom::SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader)
{
	m_InputFrameBuffer  = input;
	m_OutputFrameBuffer = output;
	m_BloomShader = shader;
}

void shade::PPBloom::SetSigma(const float& sigma)
{
	m_Sigma = sigma;
}

void shade::PPBloom::_CalculateGaussianCoef(PPBloom::ShaderData& data)
{
	
	float sigma      = m_Sigma;
	float twoSigmaSq = 2.0f * sigma * sigma;
	float r;

	float sum = 0.0f;
	for (auto i = 0u; i < GAUSSIAN_RADIUS; ++i)
	{
	
		// TODO DO RESIZE FRAME BUFFER
		//r = sqrt(i * i);
		data.Kernesl[i] = (1.0f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
		//data.Kernesl[i] = (std::exp(-(r * r) / twoSigmaSq)) / (M_PI * twoSigmaSq);
		sum += 2 * data.Kernesl[i];
	}

	sum -= data.Kernesl[0];

	float normalizationFactor = 1.0f / sum;
	for (auto i = 0u; i < GAUSSIAN_RADIUS; ++i)
		data.Kernesl[i] *= normalizationFactor;

}
