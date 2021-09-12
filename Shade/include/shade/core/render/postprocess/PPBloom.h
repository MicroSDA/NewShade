#pragma once

#include "shade/config/API.h"
#include "shade/core/render/postprocess/PostProcess.h"


namespace shade
{
	class SHADE_API PPBloom : public PostProcess
	{
	public:
		static Shared<PPBloom> Create();
		PPBloom();// ??
		void SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader);
		void SetSamplesCount(const std::uint32_t& count);
		void SetExposure(const float& exposure);
		void SetThreshold(const float& threshold);
		void SetKnee(const float& knee);
		

		std::uint32_t& GetSamplesCount();
		glm::vec3 GetCurve() const;
		float& GetThreshold();
		float& GetKnee();
		float& GetExposure();
	protected:
		Shared<FrameBuffer>   m_BloomFrameBuffer;
		Shared<FrameBuffer>   m_InputFrameBuffer;
		Shared<FrameBuffer>   m_OutputFrameBuffer;
		Shared<Shader>		  m_BloomShader;
		Shared<UniformBuffer> m_UniformBuffer;
		std::uint32_t         m_Samples;
		glm::vec3			  m_Curve;
		float				  m_Exposure;
		float				  m_Threshold;
		float				  m_Knee;
	};
}
