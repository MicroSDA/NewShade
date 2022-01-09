#include "shade_pch.h"
#include "Render.h"
#include "shade/core/vertex/Vertex3D.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLTexture.h" // !!!!!!!!TEMPORARY
namespace shade
{
	Unique<RenderAPI> Render::m_sRenderAPI = RenderAPI::Create();

	bool Render::m_sIsInit = false;
	/* Pools */
	Render::Sprites				Render::m_sSprites;
	Render::LightEnviroment		Render::m_sLightEnviroment;
	/* Buffers */
	Shared<UniformBuffer>		Render::m_sCameraUniformBuffer;
	Shared<UniformBuffer>		Render::m_sClippingUniformBuffer;
	Shared<ShaderStorageBuffer> Render::m_sDirectLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sPointLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sSpotLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sShadowCascadesBuffer;
	Shared<ShaderStorageBuffer> Render::m_sShadowSpotLightBuffer;
	Shared<FrameBuffer>			Render::m_sTargetFrameBuffer;
	Shared<FrameBuffer>			Render::m_sPreviousPassBuffer;


	Render::RenderPipelines		Render::m_sPipelines;

	std::unordered_map<Shared<Drawable>, BufferDrawData> Render::m_sInstancedGeometryBuffers;

}

void shade::Render::PProcess::Process(const Shared<PostProcess>& pp)
{
	pp->Process();
}

shade::Unique<shade::RenderAPI>& shade::Render::GetRenderApi()
{
	return m_sRenderAPI;
}

void shade::Render::Init()
{
	if (!m_sIsInit)
	{
		m_sRenderAPI->Init();
		m_sIsInit = true;

		/* Create buffers and other */
		m_sCameraUniformBuffer = UniformBuffer::Create(sizeof(Camera::RenderData), 0);
		m_sClippingUniformBuffer = UniformBuffer::Create(sizeof(glm::vec4), 1);
		m_sDirectLightsBuffer = ShaderStorageBuffer::Create(0, 2);
		m_sPointLightsBuffer = ShaderStorageBuffer::Create(0, 3);
		m_sSpotLightsBuffer = ShaderStorageBuffer::Create(0, 4);
		m_sShadowCascadesBuffer = ShaderStorageBuffer::Create(0, 5);
		m_sShadowSpotLightBuffer = ShaderStorageBuffer::Create(0, 6);

		glm::fvec2 plane[4] = { glm::fvec2(-1.0,  1.0), glm::fvec2(-1.0, -1.0) ,glm::fvec2(1.0,   1.0), glm::fvec2(1.0,  -1.0) };


		m_sSprites.VAO = VertexArray::Create();
		m_sSprites.VBO = VertexBuffer::Create(plane, sizeof(glm::fvec2) * 4, VertexBuffer::BufferType::Static);
		m_sSprites.EBO = VertexBuffer::Create(sizeof(glm::fvec2) * 4, VertexBuffer::BufferType::Dynamic);

		m_sSprites.VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float2,		 "Position"	} });
		m_sSprites.EBO->SetLayout({ {shade::VertexBuffer::ElementType::Float2,		 "UV_Coords"} });

		m_sSprites.VAO->AddVertexBuffer(m_sSprites.VBO);
		m_sSprites.VAO->AddVertexBuffer(m_sSprites.EBO);
		/*------------------------------------------------------*/
	}
	else
		SHADE_CORE_WARNING("Render API has been already initialized!");
}

void shade::Render::ShutDown()
{
	m_sRenderAPI->ShutDown();
	m_sInstancedGeometryBuffers.clear();
	m_sPipelines.Pipelines.clear();
	m_sLightEnviroment.Clear();
}

unsigned int shade::Render::GetVideoMemoryUsage()
{
	return m_sRenderAPI->GetVideoMemoryUsage();
}

void shade::Render::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	m_sRenderAPI->SetClearColor(r, g, b, a);
}

void shade::Render::Clear()
{
	m_sRenderAPI->Clear();
}

void shade::Render::DepthTest(const bool& enable)
{
	m_sRenderAPI->DepthTest(enable);
}

void shade::Render::CullFace(const int& mode)
{
	m_sRenderAPI->CullFace(mode);
}

void shade::Render::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	m_sRenderAPI->SetViewPort(x, y, width, height);
}

void shade::Render::Begin()
{
	for (auto pipeline = m_sPipelines.Pipelines.begin(); pipeline != m_sPipelines.Pipelines.end();)
	{
		for (auto drawable = pipeline->second.Drawables.begin(); drawable != pipeline->second.Drawables.end();)
		{
			for (auto mateiral = drawable->second.Materials.begin(); mateiral != drawable->second.Materials.end();)
			{
				if (!mateiral->second.size())
				{
					/* Has to be removed*/
					drawable->second.Materials.erase(mateiral++);
				}
				else
				{
					++mateiral;
				}
			}

			if (!drawable->second.Materials.size())
			{
				/* Has to be removed*/
				//m_sInstancedGeometryBuffers.erase(drawable->first); // If drawable doesnrt have material need to remove from ppol draw data
				pipeline->second.Drawables.erase(drawable++);
			}
			else
			{
				++drawable;
			}
		}

		if (!pipeline->second.Drawables.size())
		{
			/* Has to be removed*/
			m_sPipelines.Pipelines.erase(pipeline++);
		}
		else
		{
			++pipeline;
		}
	}
}

void shade::Render::End()
{
	m_sRenderAPI->End();
	m_sLightEnviroment.Clear();

	/* Clear all transform  so mark as expired */
	for (auto& pipeline : m_sPipelines.Pipelines)
	{
		for (auto& drawable : pipeline.second.Drawables)
		{
			for (auto& material : drawable.second.Materials)
			{
				material.second.clear();
			}
		}
	}
}

void shade::Render::BeginScene(const Camera::RenderData& renderData, const glm::vec4& clipping)
{
	/* Set data to uniforsm buffetrs */
	m_sCameraUniformBuffer->SetData(&renderData, sizeof(Camera::RenderData), 0);
	m_sClippingUniformBuffer->SetData(glm::value_ptr(clipping), sizeof(glm::vec4), 0);

	/* Process SSBO buffers */
	std::uint32_t dLightCount = m_sLightEnviroment.DirectLightSources.size(), pLighCount = m_sLightEnviroment.PointLightSources.size(), sLightCount = m_sLightEnviroment.SpotLightSources.size();
	/* Direct light */
	if (m_sDirectLightsBuffer->GetSize() != sizeof(DirectLight::RenderData) * dLightCount)
		m_sDirectLightsBuffer->Resize(sizeof(DirectLight::RenderData) * dLightCount);
	m_sDirectLightsBuffer->SetData(m_sLightEnviroment.DirectLightSources.data(), sizeof(DirectLight::RenderData) * dLightCount);
	/* Point light */
	if (m_sPointLightsBuffer->GetSize() != sizeof(PointLight::RenderData) * pLighCount)
		m_sPointLightsBuffer->Resize(sizeof(PointLight::RenderData) * pLighCount);
	m_sPointLightsBuffer->SetData(m_sLightEnviroment.PointLightSources.data(), sizeof(PointLight::RenderData) * pLighCount);
	/* Spot light */
	if (m_sSpotLightsBuffer->GetSize() != sizeof(SpotLight::RenderData) * sLightCount)
		m_sSpotLightsBuffer->Resize(sizeof(SpotLight::RenderData) * sLightCount);
	m_sSpotLightsBuffer->SetData(m_sLightEnviroment.SpotLightSources.data(), sizeof(SpotLight::RenderData) * sLightCount);
	/*!Process SSBO buffers*/
}

void shade::Render::BeginScene(const Shared<Camera>& camera, const Shared<FrameBuffer>& framebuffer, const glm::vec4& clipping)
{
	m_sTargetFrameBuffer = framebuffer;
	/* Set data to uniforsm buffetrs */
	m_sCameraUniformBuffer->SetData(&camera->GetRenderData(), sizeof(Camera::RenderData), 0);
	glm::vec4 clip;
	m_sClippingUniformBuffer->SetData(glm::value_ptr(clip), sizeof(glm::vec4), 0);

	/* Process SSBO buffers */
	std::uint32_t dLightCount = m_sLightEnviroment.DirectLightSources.size(), pLighCount = m_sLightEnviroment.PointLightSources.size(), sLightCount = m_sLightEnviroment.SpotLightSources.size();
	/* Shadows */
	/* Only one direct light can cast cascaded shadows */
	if (dLightCount)
	{
		/* TODO : need to redisign */
		const int cascadeCount = 4;
		//float spilt[cascadeCount] = { camera->GetFar() / 50.0f, camera->GetFar() / 25.0f, camera->GetFar() / 10.0f, camera->GetFar() / 1.0f };
		float spilt[cascadeCount];
		std::vector<Shadows::Cascade> cascades;
		m_sShadowCascadesBuffer->Resize(sizeof(Shadows::Cascade) * 4);

		const int SHADOW_MAP_CASCADE_COUNT = 4;
		float nearClip = camera->GetNear();
		float farClip = camera->GetFar();
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		float CascadeSplitLambda = clipping.z;

		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = CascadeSplitLambda * (log - uniform) + uniform;
			spilt[i] = farClip * (d - nearClip) / clipRange;
		}

		for (size_t i = 0; i < cascadeCount + 1; ++i)
		{
			if (i == 0)
				cascades.push_back(Shadows::ComputeDirectLightCascade(camera, m_sLightEnviroment.DirectLightSources[0].Direction, camera->GetNear() + clipping.x, spilt[i] + clipping.y, spilt[i]));
			else if (i < cascadeCount)
				cascades.push_back(Shadows::ComputeDirectLightCascade(camera, m_sLightEnviroment.DirectLightSources[0].Direction, spilt[i - 1] + clipping.x, spilt[i] + clipping.y, spilt[i]));
			else
				cascades.push_back(Shadows::ComputeDirectLightCascade(camera, m_sLightEnviroment.DirectLightSources[0].Direction, spilt[i - 1] + clipping.x, camera->GetFar() + clipping.y, spilt[i]));
		}

		m_sShadowCascadesBuffer->SetData(cascades.data(), sizeof(Shadows::Cascade) * 4);
	}
	/* Direct light */
	if (m_sDirectLightsBuffer->GetSize() != sizeof(DirectLight::RenderData) * dLightCount)
		m_sDirectLightsBuffer->Resize(sizeof(DirectLight::RenderData) * dLightCount);
	m_sDirectLightsBuffer->SetData(m_sLightEnviroment.DirectLightSources.data(), sizeof(DirectLight::RenderData) * dLightCount);
	/* Point light */
	if (m_sPointLightsBuffer->GetSize() != sizeof(PointLight::RenderData) * pLighCount)
		m_sPointLightsBuffer->Resize(sizeof(PointLight::RenderData) * pLighCount);
	m_sPointLightsBuffer->SetData(m_sLightEnviroment.PointLightSources.data(), sizeof(PointLight::RenderData) * pLighCount);
	/* Spot light */
	if (m_sSpotLightsBuffer->GetSize() != sizeof(SpotLight::RenderData) * sLightCount)
		m_sSpotLightsBuffer->Resize(sizeof(SpotLight::RenderData) * sLightCount);
	m_sSpotLightsBuffer->SetData(m_sLightEnviroment.SpotLightSources.data(), sizeof(SpotLight::RenderData) * sLightCount);
}

void shade::Render::EndScene()
{
	m_sRenderAPI->EndScene();
}

void shade::Render::SubmitLight(const Shared<DirectLight>& light, const glm::mat4& transform)
{
	/* Getting direction */

	m_sLightEnviroment.DirectLightSources.push_back(light->GetRenderData(-glm::normalize(glm::mat3(transform) * glm::vec3(1.0f))));
	//m_sLightEnviroment.DirectLightSources.push_back(light->GetRenderData(Transform3D::GetTransformFromMatrix(transform).GetRotation()));
}

void shade::Render::SubmitLight(const Shared<PointLight>& light, const glm::mat4& transform)
{
	m_sLightEnviroment.PointLightSources.push_back(light->GetRenderData(Transform3D::GetTransformFromMatrix(transform).GetPosition()));
}

void shade::Render::SubmitLight(const Shared<SpotLight>& light, const glm::mat4& transform)
{
	/* Getting position and direction */
	m_sLightEnviroment.SpotLightSources.push_back(light->GetRenderData(Transform3D::GetTransformFromMatrix(transform).GetPosition(), -glm::normalize(glm::mat3(transform) * glm::vec3(1.0f))));
}

void shade::Render::DrawSprite(const Shared<Shader>& shader, const Shared<Texture>& texture, const glm::mat4& transform, const glm::vec4& rectangle)
{
	glm::fvec2 uv_coords[4] = { glm::vec2(1.f / texture->GetImageData().Width * rectangle.x, 1.f / texture->GetImageData().Height * rectangle.y) ,
								glm::vec2(1.f / texture->GetImageData().Width * rectangle.x, 1.f / texture->GetImageData().Height * (rectangle.y + rectangle.w)) ,
								glm::vec2(1.f / texture->GetImageData().Width * (rectangle.x + rectangle.z), 1.f / texture->GetImageData().Height * rectangle.y),
								glm::vec2(1.f / texture->GetImageData().Width * (rectangle.x + rectangle.z), 1.f / texture->GetImageData().Height * (rectangle.y + rectangle.w)) };

	m_sSprites.EBO->SetData(uv_coords, sizeof(glm::fvec2) * 4, 0);

	texture->Bind(0);
	shader->Bind();
	shader->SendMat4("u_Transform", false, glm::value_ptr(transform));

	m_sRenderAPI->DrawNotIndexed(Drawable::DrawMode::TriangleStrip, m_sSprites.VAO, 4);
}

void shade::Render::DrawSprite(const Shared<Shader>& shader, const std::uint32_t& texture, const glm::mat4& transform, const glm::vec4& rectangle)
{
	glm::fvec2 uv_coords[4] = {
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
	};

	m_sSprites.EBO->SetData(uv_coords, sizeof(glm::fvec2) * 4, 0);

	OpenGLTexture::BindTest(texture, 0);
	shader->Bind();
	shader->SendMat4("u_Transform", false, glm::value_ptr(transform));

	m_sRenderAPI->DrawNotIndexed(Drawable::DrawMode::TriangleStrip, m_sSprites.VAO, 4);
}

void shade::Render::SubmitWithPipelineInstanced(const Shared<RenderPipeline>& pipeline, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	m_sPipelines.Pipelines[pipeline].Drawables[drawable].Materials[material].push_back(transform);

	if (m_sInstancedGeometryBuffers.find(drawable) == m_sInstancedGeometryBuffers.end())
		_CreateInstancedGeometryBuffer(drawable);
}

void shade::Render::ExecuteSubmitedPipeline(const Shared<RenderPipeline>& pipeline)
{
	auto& _pipeline = m_sPipelines.Pipelines.find(pipeline);

	if (_pipeline != m_sPipelines.Pipelines.end())
	{
		if(m_sPreviousPassBuffer)
			m_sPreviousPassBuffer = _pipeline->first->Process(m_sTargetFrameBuffer, m_sPreviousPassBuffer, _pipeline->second, m_sInstancedGeometryBuffers);
		else
			m_sPreviousPassBuffer = _pipeline->first->Process(m_sTargetFrameBuffer, m_sTargetFrameBuffer, _pipeline->second, m_sInstancedGeometryBuffers);
	}	
}

void shade::Render::ExecuteSubmitedPipelines()
{
	for (const auto& pipeline : m_sPipelines.Pipelines)
	{
		if(m_sPreviousPassBuffer)
			m_sPreviousPassBuffer = pipeline.first->Process(m_sTargetFrameBuffer, m_sPreviousPassBuffer, pipeline.second,  m_sInstancedGeometryBuffers);
		else
			m_sPreviousPassBuffer = pipeline.first->Process(m_sTargetFrameBuffer, m_sTargetFrameBuffer, pipeline.second, m_sInstancedGeometryBuffers);
	}
}

void shade::Render::Enable(const int& value)
{
	m_sRenderAPI->Enable(value);
}

void shade::Render::Disable(const int& value)
{
	m_sRenderAPI->Disable(value);
}

void shade::Render::_CreateInstancedGeometryBuffer(const Shared<Drawable>& drawable)
{
	auto& drawData = m_sInstancedGeometryBuffers[drawable];
	drawData.VAO = VertexArray::Create();
	drawData.VBO = VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Static);
	drawData.TBO = VertexBuffer::Create(0, VertexBuffer::BufferType::Dynamic);
	drawData.IBO = IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());

	drawData.VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float3, "Position"	},
							  {shade::VertexBuffer::ElementType::Float2, "UV_Coords"},
							  {shade::VertexBuffer::ElementType::Float3, "Normal"	},
							  {shade::VertexBuffer::ElementType::Float3, "Tangent"	} });
	drawData.TBO->SetLayout({ {shade::VertexBuffer::ElementType::Mat4,   "Transform"} });
	drawData.VAO->AddVertexBuffer(drawData.VBO);
	drawData.VAO->SetIndexBuffer(drawData.IBO);
	drawData.VAO->AddVertexBuffer(drawData.TBO);
}