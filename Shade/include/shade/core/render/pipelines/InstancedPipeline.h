#pragma once
#include "shade/core/render/RenderPipeline.h"

namespace shade
{
	class SHADE_API InstancedPipeline : public RenderPipeline
	{
	public:
		InstancedPipeline();
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const Shared<FrameBuffer>& GetResult() const override;
	private:
		Shared<Shader>   m_Shader;
	};
}