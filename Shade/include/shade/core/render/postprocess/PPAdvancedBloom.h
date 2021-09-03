#pragma once
#include "shade/config/API.h"
#include "shade/core/render/postprocess/PostProcess.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/Shader.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/UniformBuffer.h"

#define GAUSSIAN_RADIUS 5

namespace shade
{
	class SHADE_API PPAdvancedBloom : public PostProcess
	{
	public:
		struct ShaderData
		{
			float			 Kernesl[GAUSSIAN_RADIUS];
			int				 Radius = GAUSSIAN_RADIUS;
			//float			_pad;
		};
	public:
		static Shared<PPAdvancedBloom> Create();
		PPAdvancedBloom();
		void SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader);
		void SetSigma(const float& sigma);
	protected:
		Shared<FrameBuffer>   m_BloomFrameBuffer;
		Shared<FrameBuffer>	  m_BloomFrameBuffer2;
		Shared<FrameBuffer>   m_InputFrameBuffer;
		Shared<FrameBuffer>   m_OutputFrameBuffer;
		Shared<Shader>		  m_BloomShader;
		Shared<UniformBuffer> m_GaussianUniformBuffer;
		float				  m_Sigma = 15.0f;
		PPAdvancedBloom::ShaderData   m_Data;

		void _CalculateKernels(PPAdvancedBloom::ShaderData& data);
	};
}
