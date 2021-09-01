#include "shade_pch.h"
#include "PPBloom.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/postprocess/OpenGL_PBloom.h"

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

void shade::PPBloom::_CalculateGaussianCoef(PPBloom::ShaderData& data)
{
	
	float sigma = data.Sigma;
	float sigmaRpc = 1.0f / sigma;
	float twoSigmaSq = 2.0f * sigma * sigma;

	float sum = 0.0f;
	for (auto i = 0u; i <= GAUSSIAN_RADIUS; ++i)
	{
		data.Coefficient[i] = (1.0f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
		sum += 2 * data.Coefficient[i];
	}

	sum -= data.Coefficient[0];

	float normalizationFactor = 1.0f / sum;
	for (auto i = 0u; i <= GAUSSIAN_RADIUS; ++i)
		data.Coefficient[i] *= normalizationFactor;
}
