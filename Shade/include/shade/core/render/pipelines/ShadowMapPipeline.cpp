#include "shade_pch.h"
#include "ShadowMapPipeline.h"
#include "shade/core/render/Render.h"

shade::ShadowMapPipeline::ShadowMapPipeline(const RenderPipeline::Specification& spec)
{
	m_Specification = spec;

	m_DirectLightShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_DShadowMapMultiplier, 1024 * m_DShadowMapMultiplier, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }, 1, 4));

	m_SpotLightShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_SShadowMapMultiplier, 1024 * m_SShadowMapMultiplier, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8 }, 1, 0));

	m_PointLightShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1024 * m_PShadowMapMultiplier, 1024 * m_PShadowMapMultiplier, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8_CUBE_MAP }, 1, 0));

	m_DirectLightCascadeBuffer  = ShaderStorageBuffer::Create(sizeof(DirectLightCascade) * 4, 5);
	m_SpotLightCascadeBuffer    = ShaderStorageBuffer::Create(0,   6);
	m_PointLightCascadeBuffer   = ShaderStorageBuffer::Create(0,   7);

	shade::ShadersLibrary::Create("SpotLightShadow", "resources/shaders/General/Effects/ShadowMappingSpotLight.glsl");
	shade::ShadersLibrary::Create("PointLightShadow", "resources/shaders/General/Effects/ShadowMappingPointLight.glsl");
}

shade::Shared<shade::FrameBuffer> shade::ShadowMapPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	auto& camera =  Render::GetLastActiveCamera();
	auto& lights =  Render::GetSubmitedLight();

	/* Direc light shadows */
	if (lights.DirectLightSources.size())
	{
		const int CascadeCount = 4;
		float splitDistance[CascadeCount];

		std::vector<DirectLightCascade> cascades;
		/* Calculate split distance */
		float nearClip  = camera->GetNear();
		float farClip   = camera->GetFar();
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

		if(m_DirectLightCascadeBuffer->GetSize() != sizeof(DirectLightCascade) * cascades.size())
			m_DirectLightCascadeBuffer->Resize(sizeof(DirectLightCascade) * cascades.size());
		m_DirectLightCascadeBuffer->SetData(cascades.data(), sizeof(DirectLightCascade) * cascades.size());

		/*if (m_DirectLightShadowFrameBuffer->GetLayout().Width != target->GetLayout().Width * m_ShadowMapMultiplier || m_DirectLightShadowFrameBuffer->GetLayout().Height != target->GetLayout().Height * m_ShadowMapMultiplier)
		{
			auto layout		= m_DirectLightShadowFrameBuffer->GetLayout();
			layout.Width	= target->GetLayout().Width * m_ShadowMapMultiplier;
			layout.Height	= target->GetLayout().Height * m_ShadowMapMultiplier;
			m_DirectLightShadowFrameBuffer = FrameBuffer::Create(layout);
		}*/
	}
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

		if (m_SpotLightShadowFrameBuffer->GetLayout().Layers != lights.SpotLightSources.size())
		{
			auto layout = m_SpotLightShadowFrameBuffer->GetLayout();
			layout.Layers = lights.SpotLightSources.size();
			m_SpotLightShadowFrameBuffer = FrameBuffer::Create(layout);
		}

		if (m_SpotLightCascadeBuffer->GetSize() != sizeof(SpotLightCascade) * spotLightCascades.size())
			m_SpotLightCascadeBuffer->Resize(sizeof(SpotLightCascade) * spotLightCascades.size());
		m_SpotLightCascadeBuffer->SetData(spotLightCascades.data(), sizeof(SpotLightCascade) * spotLightCascades.size());
		
	}
	/* Point light shadows */
	if (lights.PointLightSources.size())
	{
		std::vector<PointLightCascade> pointLightCascades;
		for (auto& light : lights.PointLightSources)
		{
			constexpr float fov = glm::radians(90.0f);
			float distance = light.Distance + 1.0f;
			float _near = 1.0f;
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0, -1.0,  0.0), _near, distance));	// x
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0,  0.0), _near, distance));	//-x
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0,  0.0,  1.0), _near, distance));	// y
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0,  0.0, -1.0), _near, distance));	//-y
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0, -1.0,  0.0), _near, distance));	// z
			pointLightCascades.push_back(ComputePointLightCascade(fov, light.Position, glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0, -1.0,  0.0), _near, distance));	//-z
		}

		/*if (m_PointLightShadowFrameBuffer->GetLayout().Layers != lights.PointLightSources.size() * 6)
		{
			auto layout = m_PointLightShadowFrameBuffer->GetLayout();
			layout.Layers = lights.PointLightSources.size() * 6;
			m_PointLightShadowFrameBuffer = FrameBuffer::Create(layout);
		}*/

		if (m_PointLightCascadeBuffer->GetSize() != sizeof(PointLightCascade) * pointLightCascades.size())
			m_PointLightCascadeBuffer->Resize(sizeof(PointLightCascade) * pointLightCascades.size());
		m_PointLightCascadeBuffer->SetData(pointLightCascades.data(), sizeof(PointLightCascade) * pointLightCascades.size());
	}
	/* Render */
	auto& shader = m_Specification.Shader;
	shader->Bind(); shader->ExecuteSubrutines();
	m_DirectLightShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
	/*  Before you render objects to the CSM, call glEnable(GL_DEPTH_CLAMP); and after, call glDisable(GL_DEPTH_CLAMP);
		This will cause every object that gets rendered to not get cut out, so artifacts where some cascades cut out some geometry will disappear. */
	Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
	if (lights.DirectLightSources.size() && m_IsDirectShadowCast)
	{
		for (auto& [instance, materials] : drawables.Drawables)
		{
			for (auto& [material, transforms] : materials.Materials)
			{
				drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
				drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

				Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
			}
		}
	}
	if (lights.SpotLightSources.size() && m_IsSpotShadowCast)
	{
		/* Spot light */
		ShadersLibrary::Get("SpotLightShadow")->Bind();
		m_SpotLightShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
		for (auto& [instance, materials] : drawables.Drawables)
		{
			for (auto& [material, transforms] : materials.Materials)
			{
				drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
				drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

				Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
			}
		}
	}
	if (lights.PointLightSources.size() && m_IsPointShadowCast)
	{

		/* Point light */
		ShadersLibrary::Get("PointLightShadow")->Bind();
		m_PointLightShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
		for (auto& [instance, materials] : drawables.Drawables)
		{
			for (auto& [material, transforms] : materials.Materials)
			{
				drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
				drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

				Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
			}
		}
	}
	Render::GetRenderApi()->Disable(0x864F);
	shader->UnBind();

	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::ShadowMapPipeline::GetResult() const
{
	return m_DirectLightShadowFrameBuffer;
}

const shade::Shared<shade::FrameBuffer>& shade::ShadowMapPipeline::GetDriectLightFrameBuffer()
{
	return m_DirectLightShadowFrameBuffer;
}

const shade::Shared<shade::FrameBuffer>& shade::ShadowMapPipeline::GetSpotLightFrameBuffer()
{
	return m_SpotLightShadowFrameBuffer;
}

const shade::Shared<shade::FrameBuffer>& shade::ShadowMapPipeline::GetPointLightFrameBuffer()
{
	return m_PointLightShadowFrameBuffer;
}

void shade::ShadowMapPipeline::SetDirectLightShadowCast(const bool isCast)
{
	m_IsDirectShadowCast = isCast;
}

void shade::ShadowMapPipeline::SetSpotLightShadowCast(const bool isCast)
{
	m_IsSpotShadowCast = isCast;
}

void shade::ShadowMapPipeline::SetPointLightShadowCast(const bool isCast)
{
	m_IsPointShadowCast = isCast;
}

shade::ShadowMapPipeline::DirectLightCascade shade::ShadowMapPipeline::ComputeDirectLightCascade(const shade::Shared<Camera>& camera, const glm::vec3& direction, const float& nearPlane, const float& farplane, const float& split)
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

shade::ShadowMapPipeline::SpotLightCascade shade::ShadowMapPipeline::ComputeSpotLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const float& nearPlane, const float& farplane)
{
	glm::mat4 lightProjection = glm::perspective(fov, 1.0f, nearPlane, farplane);
	glm::mat4 lightView       = glm::lookAt(position - direction, position, glm::vec3(0.0, 1.0, 0.0));

	return SpotLightCascade{ lightProjection * lightView , farplane };
}

shade::ShadowMapPipeline::PointLightCascade shade::ShadowMapPipeline::ComputePointLightCascade(const float& fov, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float& nearPlane, const float& farplane)
{
	glm::mat4 lightProjection = glm::perspective(fov, 1.0f, nearPlane, farplane);
	glm::mat4 lightView = glm::lookAt(position, position + direction, up);

	return PointLightCascade{ lightProjection * lightView, farplane };
}

std::vector<glm::vec4> shade::ShadowMapPipeline::GetCameraFrustumCorners(const glm::mat4& projection, const glm::mat4& veiw)
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
