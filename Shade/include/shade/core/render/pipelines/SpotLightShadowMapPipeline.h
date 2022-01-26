#pragma once
#include "shade/core/render/RenderPipeline.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/camera/Camera.h"

namespace shade
{
	class SHADE_API SpotLightShadowMapPipeline : public RenderPipeline
	{
	public:
		struct SpotLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16)	float	  Distance = 0.0f;
		};
	public:
		SpotLightShadowMapPipeline();
		virtual ~SpotLightShadowMapPipeline();
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const Shared<FrameBuffer>& GetResult() const override;
	private:
		Shared<FrameBuffer>         m_ShadowFrameBuffer;
		Shared<ShaderStorageBuffer>	m_CascadeBuffer;
		Shared<Shader>				m_Shader;
		float						m_ShadowMapMultiplier = 2.0f;
		bool						m_IsShadowCast		  = true;
	private:
		SpotLightCascade     ComputeSpotLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const float& nearPlane, const float& farplane);
	};
}