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

shade::PPBloom::PPBloom() :
	m_Sigma(15.f), m_Scaling(8), m_Curve(0.2126f, 0.7152f, 0.0722f), m_Threshold(1.0), m_Radius(MAX_GAUSSIAN_RADIUS)
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		}, m_Scaling));

	_CalculateKernels();
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
	_CalculateKernels();
}

void shade::PPBloom::SetScaling(const std::uint32_t& scaling)
{
	if (scaling <= 10)
		m_Scaling = scaling;
	else
		SHADE_CORE_WARNING("Wrong scaling value, max: 8, provided: {0}.", scaling);
}

void shade::PPBloom::SetRadius(const std::uint32_t& radius)
{
	if (radius <= MAX_GAUSSIAN_RADIUS)
		m_Radius = radius;
	else
		SHADE_CORE_WARNING("Wrong gaussian radius, max : {0}, provided: {1}.", MAX_GAUSSIAN_RADIUS, radius);
}

void shade::PPBloom::SetCurve(glm::vec3& curve)
{
	m_Curve = curve;
}

void shade::PPBloom::SetThreshold(const float& threshold)
{
	m_Threshold = threshold;
}

std::uint32_t& shade::PPBloom::GetScaling()
{
	return m_Scaling;
}

std::uint32_t& shade::PPBloom::GetRadius()
{
	return m_Radius;
}

glm::vec3& shade::PPBloom::GetCurve()
{
	return m_Curve;
}

float& shade::PPBloom::GetThreshold()
{
	return m_Threshold;
}

void shade::PPBloom::_CalculateKernels()
{
	
	float sigma      = m_Sigma;
	float twoSigmaSq = 2.0f * sigma * sigma;
	float r;

	float sum = 0.0f;
	for (auto i = 0u; i < MAX_GAUSSIAN_RADIUS; ++i)
	{
		m_Kernesl[i] = (1.0f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
		sum += 2 * m_Kernesl[i];
	}

	sum -= m_Kernesl[0];

	float normalizationFactor = 1.0f / sum;
	for (auto i = 0u; i < MAX_GAUSSIAN_RADIUS; ++i)
		m_Kernesl[i] *= normalizationFactor;

}
