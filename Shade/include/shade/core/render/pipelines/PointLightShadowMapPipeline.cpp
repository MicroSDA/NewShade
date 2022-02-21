#include "shade_pch.h"
#include "PointLightShadowMapPipeline.h"
#include "shade/core/render/Render.h"

shade::PointLightShadowMapPipeline::PointLightShadowMapPipeline()
{
	m_ShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_ShadowMapMultiplier, 1024 * m_ShadowMapMultiplier, {
	shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8_CUBE_MAP }, 1, 1));
	m_CascadeBuffer  = ShaderStorageBuffer::Create(0, 9);
	m_SettingsBuffer = UniformBuffer::Create(sizeof(Settings), 10);

	m_Shader = ShadersLibrary::Create("PlShadowMapping", "resources/shaders/General/Effects/ShadowMappingPointLight.glsl");
}

shade::PointLightShadowMapPipeline::~PointLightShadowMapPipeline()
{

}

shade::Shared<shade::FrameBuffer> shade::PointLightShadowMapPipeline::Process(const shade::Shared<shade::FrameBuffer>& target, const shade::Shared<shade::FrameBuffer>& previousPass, const shade::Shared<shade::RenderPipeline>& previusPipline, const shade::DrawablePools& drawables, std::unordered_map<shade::Shared<shade::Drawable>, shade::BufferDrawData, std::hash<shade::Shared<shade::Drawable>>, std::equal_to<shade::Shared<shade::Drawable>>, std::allocator<std::pair<const shade::Shared<shade::Drawable>, shade::BufferDrawData>>>& drawData)
{
	m_SettingsBuffer->SetData(&m_Settings, sizeof(Settings));

	if (m_Settings.IsShadowCast)
	{
		auto& camera = Render::GetLastActiveCamera();
		auto& lights = Render::GetSubmitedLight();
		/* Point light shadows */
		if (lights.PointLightSources.size())
		{
			std::vector<PointLightCascade> pointLightCascades;
			constexpr float fov = glm::radians(90.0f);
			float _near = 0.1f;
			float aspect = 1.0f;
			for (auto& light : lights.PointLightSources)
			{
				float distance = light.Distance + 1.0f;
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0), _near, distance));	// x
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0), _near, distance));	//-x
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0), _near, distance));	// y
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0), _near, distance));	//-y
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0), _near, distance));	// z
				pointLightCascades.push_back(ComputePointLightCascade(fov, aspect, light.Position, glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0), _near, distance));	//-z
			}

			if (m_ShadowFrameBuffer->GetLayout().Layers != lights.PointLightSources.size())
			{
				auto layout = m_ShadowFrameBuffer->GetLayout();
				layout.Layers = lights.PointLightSources.size();
				m_ShadowFrameBuffer = FrameBuffer::Create(layout);
			}

			if (m_CascadeBuffer->GetSize() != sizeof(PointLightCascade) * pointLightCascades.size())
				m_CascadeBuffer->Resize(sizeof(PointLightCascade) * pointLightCascades.size());
			m_CascadeBuffer->SetData(pointLightCascades.data(), sizeof(PointLightCascade) * pointLightCascades.size());

			/* Render */
			m_Shader->Bind();
			m_ShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
			//Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
			for (auto& [instance, materials] : drawables.Drawables)
			{
				for (auto& [material, transforms] : materials.Materials)
				{
					if (drawData[instance].TBO->GetSize() != sizeof(glm::mat4) * transforms.size())
						drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
					drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

					Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
				}
			}
			//Render::GetRenderApi()->Disable(0x864F); // DEPTH_CLAMP

			/* Bind shadow map as texture */
			m_ShadowFrameBuffer->BindDepthAsTexture(5);
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

shade::PointLightShadowMapPipeline::PointLightCascade shade::PointLightShadowMapPipeline::ComputePointLightCascade(const float& fov, const float& apsect, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float& nearPlane, const float& farplane)
{
	glm::mat4 lightProjection = glm::perspective(fov, apsect, nearPlane, farplane);
	glm::mat4 lightView = glm::lookAt(position, position + direction, up);

	return PointLightCascade{ lightProjection * lightView, farplane };
}

const shade::Shared<shade::FrameBuffer>& shade::PointLightShadowMapPipeline::GetResult() const
{
	return nullptr;
}

void shade::PointLightShadowMapPipeline::SetSettings(const PointLightShadowMapPipeline::Settings& settings)
{
	m_Settings = settings;
}

const shade::PointLightShadowMapPipeline::Settings& shade::PointLightShadowMapPipeline::GetSettings() const
{
	return m_Settings;
}

void shade::PointLightShadowMapPipeline::SetMultiplier(const float& multiplier)
{
	m_ShadowMapMultiplier = multiplier;
	auto layout = m_ShadowFrameBuffer->GetLayout();
	layout.Width  = 1024 * m_ShadowMapMultiplier;
	layout.Height = 1024 * m_ShadowMapMultiplier;
	m_ShadowFrameBuffer = shade::FrameBuffer::Create(layout);
}
