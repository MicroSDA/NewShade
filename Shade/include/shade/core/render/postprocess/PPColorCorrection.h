#pragma once
#include "shade/core/render/postprocess/PostProcess.h"

namespace shade
{
	class SHADE_API PPColorCorrection : public PostProcess
	{
	public:
		enum class ToneMapping
		{
			None,
			ACES
		};
	public:
		static Shared<PPColorCorrection> Create();
		PPColorCorrection();// ?
		void SetInOutTargets(const Shared<FrameBuffer>& input, const Shared<FrameBuffer>& output, const Shared<Shader>& shader);

		void SetExposure(const float& exposure);
		void SetGamma(const float& gamma);

		const float& GetExposure() const;
		const float& GetGamma() const;

		float& GetExposure();
		float& GetGamma();
	protected:
		Shared<FrameBuffer>   m_InputFrameBuffer;
		Shared<FrameBuffer>   m_OutputFrameBuffer;
		Shared<Shader>		  m_Shader;

		float m_Gamma;
		float m_Exposure;
		ToneMapping m_ToneMapping;
	};
}