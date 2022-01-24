#pragma once
#include "shade/core/render/RenderPipeline.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/camera/Camera.h"

namespace shade
{
	class SHADE_API ShadowMapPipeline : public RenderPipeline
	{
	public:
		struct DirectLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16) float     ClipDistacne = 0.0f;
		};
		struct SpotLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16)	float	  Distance = 0.0f;
		};
		struct PointLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16)	float     Distance = 0.0f;
		};
	public:
		ShadowMapPipeline();
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const Shared<FrameBuffer>& GetResult() const override;
		const Shared<FrameBuffer>& GetDriectLightFrameBuffer();
		const Shared<FrameBuffer>& GetSpotLightFrameBuffer();
		const Shared<FrameBuffer>& GetPointLightFrameBuffer();

		void SetDirectLightShadowCast(const bool isCast);
		void SetSpotLightShadowCast(const bool isCast);
		void SetPointLightShadowCast(const bool isCast);
	private:
		
		Shared<FrameBuffer>         m_DirectLightShadowFrameBuffer;
		Shared<FrameBuffer>         m_SpotLightShadowFrameBuffer;
		Shared<FrameBuffer>         m_PointLightShadowFrameBuffer;

		Shared<ShaderStorageBuffer>	m_DirectLightCascadeBuffer;
		Shared<ShaderStorageBuffer>	m_SpotLightCascadeBuffer;
		Shared<ShaderStorageBuffer>	m_PointLightCascadeBuffer;

		Shared<Shader> m_DShader;
		Shared<Shader> m_SShader;
		Shared<Shader> m_PShader;

		float						m_DShadowMapMultiplier = 4.0f;
		float						m_SShadowMapMultiplier = 2.0f;
		float						m_PShadowMapMultiplier = 2.0f;
		
		bool m_IsDirectShadowCast  = true;
		bool m_IsSpotShadowCast    = true;
		bool m_IsPointShadowCast   = true;

	private:
		DirectLightCascade   ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction, const float& nearPlane, const float& farplane, const float& split);
		SpotLightCascade     ComputeSpotLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const float& nearPlane, const float& farplane);
		PointLightCascade    ComputePointLightCascade(const float& fov, const float& apsect, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float& nearPlane, const float& farplane);
		std::vector<glm::vec4>  GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw);
	};
}