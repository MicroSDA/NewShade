#pragma once

#include "shade/config/API.h"
#include "shade/core/render/postprocess/PostProcess.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/Shader.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	class SHADE_API PPBloom : public PostProcess
	{
	public:
		static Shared<PPBloom> Create();
		PPBloom();
		void SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader);		
		void SetThreshold(const float& threshold);
		void SetSamplesCount(const std::uint32_t& count);
		void SetKnee(const float& knee);

		std::uint32_t& GetSamplesCount();
		glm::vec3 GetCurve() const;
		float& GetThreshold();
		float& GetKnee();

	protected:
		Shared<FrameBuffer>   m_BloomFrameBuffer;
		Shared<FrameBuffer>   m_InputFrameBuffer;
		Shared<FrameBuffer>   m_OutputFrameBuffer;
		Shared<Shader>		  m_BloomShader;

		std::uint32_t         m_Samples;
		float                 m_Threshold;
		float				  m_Knee;
	};
}
