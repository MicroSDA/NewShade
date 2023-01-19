#include "shade_pch.h"
#include "DirectLightShadowMapPipeline.h"
#include "shade/core/render/Render.h"

shade::DirectLightShadowMapPipeline::DirectLightShadowMapPipeline()
{
	m_ShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_ShadowMapMultiplier, 1024 * m_ShadowMapMultiplier, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }, 1, 4));

	m_Shader = ShadersLibrary::Create("DlShadowMapping", "resources/shaders/General/Effects/ShadowMappingDirectLight.glsl");
	m_CascadeBuffer = ShaderStorageBuffer::Create(sizeof(DirectLightCascade) * 4, 5);
	m_SettingsBuffer = UniformBuffer::Create(sizeof(Settings), 6);
}

shade::DirectLightShadowMapPipeline::~DirectLightShadowMapPipeline()
{
}

shade::Shared<shade::FrameBuffer> shade::DirectLightShadowMapPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_SettingsBuffer->SetData(&m_Settings, sizeof(Settings));

	if (m_Settings.IsShadowCast)
	{
		auto& camera = Render::GetLastActiveCamera();
		auto& lights = Render::GetSubmitedLight();
		if (lights.DirectLightSources.size())
		{
			const int CascadeCount = 4;
			float splitDistance[CascadeCount];

			std::vector<DirectLightCascade> cascades;
			/* Calculate split distance */
			float nearClip = camera->GetNear();
			float farClip = camera->GetFar();
			float clipRange = farClip - nearClip;

			glm::vec2 MinMaxZ(nearClip, nearClip + clipRange);

			float range = MinMaxZ.y - MinMaxZ.x;
			float ratio = MinMaxZ.y / MinMaxZ.x;
			float splitValue = 0.9;

			for (uint32_t i = 0; i < CascadeCount; i++)
			{
				float p = (i + 1) / static_cast<float>(CascadeCount);
				float log = MinMaxZ.x * std::pow(ratio, p);
				float uniform = MinMaxZ.x + range * p;
				float d = splitValue * (log - uniform) + uniform;
				splitDistance[i] = farClip * (d - nearClip) / clipRange;
			}

			for (auto i = 0; i < CascadeCount + 1; i++)
			{
				if (i == 0)
					cascades.push_back(ComputeDirectLightCascade(camera, lights.DirectLightSources[0].Direction, camera->GetNear(), splitDistance[i], splitDistance[i]));
				else if (i < CascadeCount)
					cascades.push_back(ComputeDirectLightCascade(camera, lights.DirectLightSources[0].Direction, splitDistance[i - 1], splitDistance[i], splitDistance[i]));
				else
					cascades.push_back(ComputeDirectLightCascade(camera, lights.DirectLightSources[0].Direction, splitDistance[i - 1], camera->GetFar(), splitDistance[i]));
			}

			if (m_ShadowFrameBuffer->GetLayout().Layers != CascadeCount)
			{
				auto layout = m_ShadowFrameBuffer->GetLayout();
				layout.Layers = CascadeCount;
				m_ShadowFrameBuffer = FrameBuffer::Create(layout);
			}
			/* Fill buffer */
			if (m_CascadeBuffer->GetSize() != sizeof(DirectLightCascade) * cascades.size())
				m_CascadeBuffer->Resize(sizeof(DirectLightCascade) * cascades.size());
			m_CascadeBuffer->SetData(cascades.data(), sizeof(DirectLightCascade) * cascades.size());

			/* Render */
			m_Shader->Bind();
			m_ShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);

			/*  Before you render objects to the CSM, call glEnable(GL_DEPTH_CLAMP); and after, call glDisable(GL_DEPTH_CLAMP);
			This will cause every object that gets rendered to not get cut out, so artifacts where some cascades cut out some geometry will disappear. */
			Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
			for (auto& [instance, materials] : drawables.Drawables)
			{
				for (auto& [material, transforms] : materials.Materials)
				{
					/* To check alpha */
					if (material)
						if (material->TextureDiffuse)
							material->TextureDiffuse->Bind(0);

					if (drawData[instance].TBO->GetSize() != sizeof(glm::mat4) * transforms.size())
						drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
					drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

					Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
				}
			}
			Render::GetRenderApi()->Disable(0x864F);
		}
		/* Bind shadow map as texture */
		m_ShadowFrameBuffer->BindDepthAsTexture(3);
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

const shade::Shared<shade::FrameBuffer>& shade::DirectLightShadowMapPipeline::GetResult() const
{
	return nullptr;
}

void shade::DirectLightShadowMapPipeline::SetSettings(const DirectLightShadowMapPipeline::Settings& settings)
{
	m_Settings = settings;
}

const shade::DirectLightShadowMapPipeline::Settings& shade::DirectLightShadowMapPipeline::GetSettings() const
{
	return m_Settings;
}

void shade::DirectLightShadowMapPipeline::SetMultiplier(const float& multiplier)
{
	m_ShadowMapMultiplier = multiplier;
	auto layout = m_ShadowFrameBuffer->GetLayout();
	layout.Width = 1024 * m_ShadowMapMultiplier;
	layout.Height = 1024 * m_ShadowMapMultiplier;
	m_ShadowFrameBuffer = shade::FrameBuffer::Create(layout);
}

shade::DirectLightShadowMapPipeline::DirectLightCascade  shade::DirectLightShadowMapPipeline::ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction, const float& nearPlane, const float& farplane, const float& split)
{
	glm::mat4 projection = glm::perspective(camera->GetFov(), camera->GetAspect(), nearPlane, farplane);

	auto frustumCorners = GetCameraFrustumCorners(projection, camera->GetView());
	glm::vec3 frustumCenter(0.0f);
	/* Get frustum center */
	for (const auto& c : frustumCorners)
	{
		frustumCenter += glm::vec3(c);
	}
	frustumCenter /= 8.0f; // 8

	float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::min(),
		minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::min(),
		minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::min();

	/* Create veiw matrix based on camera frustum and light direction */			            /* Up vector */
	const glm::mat4 lightView = glm::lookAt(frustumCenter - direction, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

	for (const auto& c : frustumCorners)
	{
		const auto trf = lightView * c;
		minX = std::min(minX, trf.x); maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y); maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z); maxZ = std::max(maxZ, trf.z);
	}

	float radius = 0.0f;

	for (uint32_t i = 0; i < 8; i++)
	{
		float distance = glm::length(frustumCorners[i] - glm::vec4(frustumCenter, 1.0));
		radius = glm::max(radius, distance);
	}
	radius = std::ceil(radius * 16.0f) / 16.0f;

	glm::vec3 maxExtents = glm::vec3(radius);
	glm::vec3 minExtents = -maxExtents;

	/* Create ortho projection based on camera frustum corners */

	const glm::mat4 lightProjection = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, minExtents.z, maxExtents.z - minExtents.z);
	return  DirectLightCascade{ lightProjection * lightView, radius };
}

std::vector<glm::vec4> shade::DirectLightShadowMapPipeline::GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw)
{
	/* Create new view projection based on the split distance*/
	glm::mat4 cameraViewProjection = glm::inverse(projection * veiw);

	/* Get camera frustum corners */
	std::vector<glm::vec4> corners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt = cameraViewProjection * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				corners.push_back(pt / pt.w);
			}
		}
	}
	return corners;
}
