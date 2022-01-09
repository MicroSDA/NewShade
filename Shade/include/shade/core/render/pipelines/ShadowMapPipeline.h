#pragma once
#include "shade/core/render/RenderPipeline.h"

namespace shade
{
	class SHADE_API ShadowMapPipeline : public RenderPipeline
	{
	public:
		ShadowMapPipeline(const RenderPipeline::Specification& spec);
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const Shared<FrameBuffer>& GetResult() const override;
	private:
		Shared<FrameBuffer> m_DirectLightShadowFrameBuffer;
	};
}