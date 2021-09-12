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

shade::PPBloom::PPBloom() :
	m_Samples(5), m_Exposure(1.0f), m_Threshold(1.f), m_Knee(0.1f)
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		}, m_Samples));

}

void shade::PPBloom::SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader)
{
	m_InputFrameBuffer  = input;
	m_OutputFrameBuffer = output;
	m_BloomShader = shader;
}

void shade::PPBloom::SetThreshold(const float& threshold)
{
	m_Threshold = threshold;
}

void shade::PPBloom::SetSamplesCount(const std::uint32_t& count)
{
	if (count <= 10)
		m_Samples = count;
	else
		SHADE_CORE_WARNING("Wrong samples value, max: 10, provided: {0}.", count);
}

void shade::PPBloom::SetKnee(const float& knee)
{
	m_Knee = knee;
}

void shade::PPBloom::SetExposure(const float& exposure)
{
	m_Exposure = exposure;
}

std::uint32_t& shade::PPBloom::GetSamplesCount()
{
	return m_Samples;
}

glm::vec3 shade::PPBloom::GetCurve() const
{
	return glm::vec3(m_Threshold - m_Knee, m_Knee * 2, 0.25f/ m_Knee);
}

float& shade::PPBloom::GetThreshold()
{
	return m_Threshold;
}

float& shade::PPBloom::GetKnee()
{
	return m_Knee;
}

float& shade::PPBloom::GetExposure()
{
	return m_Exposure;
}


