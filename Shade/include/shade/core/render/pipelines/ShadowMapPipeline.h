#pragma once
#include "shade/core/render/RenderPipeline.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/camera/Camera.h"

namespace shade
{
	class SHADE_API ShadowMapPipeline : public RenderPipeline
	{
	public:
		struct Cascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16) float     ClipDistacne = 0.0f;
		};
	public:
		ShadowMapPipeline(const RenderPipeline::Specification& spec);
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const Shared<FrameBuffer>& GetResult() const override;

		const Shared<FrameBuffer>& GetDriectLightFrameBuffer();
		const Shared<FrameBuffer>& GetSpotLightFrameBuffer();

	private:
		
		Shared<FrameBuffer>         m_DirectLightShadowFrameBuffer;
		Shared<FrameBuffer>         m_SpotLightShadowFrameBuffer;

		Shared<ShaderStorageBuffer>	m_DirectLightCascadeBuffer;
		Shared<ShaderStorageBuffer>	m_SpotLightCascadeBuffer;

	private:
		Cascade   ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction, const float& nearPlane, const float& farplane, const float& split);
		glm::mat4 ComputeSpotLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const float& nearPlane, const float& farplane);
		std::vector<glm::vec4>  GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw);
	};
}