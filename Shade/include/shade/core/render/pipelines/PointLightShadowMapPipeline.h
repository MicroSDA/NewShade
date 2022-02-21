#pragma once
#include "shade/core/render/RenderPipeline.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include "shade/core/camera/Camera.h"

namespace shade
{
	class SHADE_API PointLightShadowMapPipeline : public RenderPipeline
	{
	public:
		struct PointLightCascade
		{
			alignas(16) glm::mat4 ViewMatrix;
			alignas(16)	float     Distance = 0.0f;
		};
		struct Settings
		{
			alignas(16) bool IsShadowCast = true;
		};
		PointLightShadowMapPipeline();
		virtual ~PointLightShadowMapPipeline();
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) override;
		virtual const shade::Shared<shade::FrameBuffer>& GetResult() const override;
		void SetSettings(const PointLightShadowMapPipeline::Settings& settings);
		const PointLightShadowMapPipeline::Settings& GetSettings() const;
		void SetMultiplier(const float& multiplier);
	private:
		Shared<FrameBuffer>         m_ShadowFrameBuffer;
		Shared<ShaderStorageBuffer>	m_CascadeBuffer;
		Shared<UniformBuffer>		m_SettingsBuffer;
		Shared<Shader>				m_Shader;

		Settings					m_Settings;
		float						m_ShadowMapMultiplier = 1.0f;
		PointLightCascade    ComputePointLightCascade(const float& fov, const float& apsect, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float& nearPlane, const float& farplane);
	};
}
