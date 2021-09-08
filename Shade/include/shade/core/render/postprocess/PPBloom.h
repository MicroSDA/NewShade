#pragma once

#include "shade/config/API.h"
#include "shade/core/render/postprocess/PostProcess.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/Shader.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_GAUSSIAN_RADIUS 8

namespace shade
{
	class SHADE_API PPBloom : public PostProcess
	{
	public:
		static Shared<PPBloom> Create();
		PPBloom();
		void SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader);
		void SetSigma(const float& sigma);
		void SetScaling(const std::uint32_t& scaling);
		void SetRadius(const std::uint32_t& radius);
		void SetCurve(glm::vec3& curve);
		void SetThreshold(const float& threshold);

		std::uint32_t& GetScaling();
		std::uint32_t& GetRadius();
		glm::vec3& GetCurve();
		float& GetThreshold();

	protected:
		Shared<FrameBuffer>   m_BloomFrameBuffer;
		Shared<FrameBuffer>   m_InputFrameBuffer;
		Shared<FrameBuffer>   m_OutputFrameBuffer;
		Shared<Shader>		  m_BloomShader;
		float				  m_Sigma;
		std::uint32_t         m_Scaling; // no more than 8
		glm::vec3			  m_Curve;
		float                 m_Threshold;
		float			      m_Kernesl[MAX_GAUSSIAN_RADIUS];
		std::uint32_t		  m_Radius;
		void _CalculateKernels();
	};
}
