#include "shade_pch.h"
#include "SpotLightShadowMapPipeline.h"
#include "shade/core/render/Render.h"

shade::SpotLightShadowMapPipeline::SpotLightShadowMapPipeline()
{
	m_ShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_ShadowMapMultiplier, 1024 * m_ShadowMapMultiplier, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }, 1, 0));
	m_CascadeBuffer    = ShaderStorageBuffer::Create(0,   7);
	m_SettingsBuffer = UniformBuffer::Create(sizeof(Settings), 8);

	m_Shader = ShadersLibrary::Create("SlShadowMapping","resources/shaders/General/Effects/ShadowMappingSpotLight.glsl");
}

shade::SpotLightShadowMapPipeline::~SpotLightShadowMapPipeline()
{
}

shade::Shared<shade::FrameBuffer> shade::SpotLightShadowMapPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_SettingsBuffer->SetData(&m_Settings, sizeof(Settings));

	if (m_Settings.IsShadowCast)
	{
		auto& camera = Render::GetLastActiveCamera();
		auto& lights = Render::GetSubmitedLight();

		/* Spot light shadows */
		if (lights.SpotLightSources.size())
		{
			std::vector<SpotLightCascade> spotLightCascades;

			for (auto& light : lights.SpotLightSources)
			{
				float fov = glm::acos(light.MaxAngle) * 2.0;
				float distance = light.Distance + 1.0f;
				spotLightCascades.push_back(ComputeSpotLightCascade(fov, light.Position, light.Direction, 1.0f, distance));
			}

			if (m_ShadowFrameBuffer->GetLayout().Layers != lights.SpotLightSources.size())
			{
				auto layout = m_ShadowFrameBuffer->GetLayout();
				layout.Layers = lights.SpotLightSources.size();
				m_ShadowFrameBuffer = FrameBuffer::Create(layout);
			}

			if (m_CascadeBuffer->GetSize() != sizeof(SpotLightCascade) * spotLightCascades.size())
				m_CascadeBuffer->Resize(sizeof(SpotLightCascade) * spotLightCascades.size());
			m_CascadeBuffer->SetData(spotLightCascades.data(), sizeof(SpotLightCascade) * spotLightCascades.size());

			/* Render */
			m_Shader->Bind();
			m_ShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
			Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
			for (auto& [instance, materials] : drawables.Drawables)
			{
				for (auto& [material, transforms] : materials.Materials)
				{
					if (material)
					{
						/* To check alpha */
						if (material->TextureDiffuse)
							material->TextureDiffuse->Bind(0);
					}
					if (drawData[instance].TBO->GetSize() != sizeof(glm::mat4) * transforms.size())
						drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
					drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

					Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
				}
			}
			Render::GetRenderApi()->Disable(0x864F);

			/* Bind shadow map as texture */
			m_ShadowFrameBuffer->BindDepthAsTexture(4);
		}
	}
	else
	{
		/* Reset frame buffer */
		auto layout = m_ShadowFrameBuffer->GetLayout();
		if (layout.Layers != 0)
		{
			layout.Layers = 0;
			m_ShadowFrameBuffer = shade::FrameBuffer::Create(layout);
		}
	}
	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::SpotLightShadowMapPipeline::GetResult() const
{
	return nullptr;
}

void shade::SpotLightShadowMapPipeline::SetSettings(const SpotLightShadowMapPipeline::Settings& settings)
{
	m_Settings = settings;
}

const shade::SpotLightShadowMapPipeline::Settings& shade::SpotLightShadowMapPipeline::GetSettings() const
{
	return m_Settings;
}

shade::SpotLightShadowMapPipeline::SpotLightCascade shade::SpotLightShadowMapPipeline::ComputeSpotLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const float& nearPlane, const float& farplane)
{
	glm::mat4 lightProjection = glm::perspective(fov, 1.0f, nearPlane, farplane);
	glm::mat4 lightView       = glm::lookAt(position - direction, position, glm::vec3(0.0, 1.0, 0.0));

	return SpotLightCascade{ lightProjection * lightView , farplane };
}