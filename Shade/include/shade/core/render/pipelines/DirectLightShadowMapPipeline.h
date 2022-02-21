#pragma once
#include "shade/core/render/RenderPipeline.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/camera/Camera.h"
#include "shade/core/render/buffers/UniformBuffer.h"

namespace shade
{
	class SHADE_API DirectLightShadowMapPipeline : public RenderPipeline
	{
	public:
		struct DirectLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16) float     ClipDistacne = 0.0f;
		};
		struct Settings
		{
			alignas(16) bool IsShadowCast = true;
		};
		DirectLightShadowMapPipeline();
		virtual ~DirectLightShadowMapPipeline();
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const shade::Shared<shade::FrameBuffer>& GetResult() const override;
		void SetSettings(const DirectLightShadowMapPipeline::Settings& settings);
		const DirectLightShadowMapPipeline::Settings& GetSettings() const;
	private:
		Shared<FrameBuffer>         m_ShadowFrameBuffer;
		Shared<ShaderStorageBuffer>	m_CascadeBuffer;
		Shared<Shader>				m_Shader;
		Shared<UniformBuffer>		m_SettingsBuffer;

		float						m_ShadowMapMultiplier	= 4.0f;
		Settings					m_Settings;
	
		DirectLightCascade		ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction, const float& nearPlane, const float& farplane, const float& split);
		std::vector<glm::vec4>  GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw);
	};
}