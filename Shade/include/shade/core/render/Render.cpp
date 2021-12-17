#include "shade_pch.h"
#include "Render.h"
#include "shade/core/vertex/Vertex3D.h"

namespace shade
{
	Unique<RenderAPI> Render::m_sRenderAPI = RenderAPI::Create();

	bool Render::m_sIsInit = false;
	/* Pools */
	Render::InstancePool		Render::m_sInstancePool;
	Render::SubmitedPool		Render::m_sSubmitedPool;
	Render::Sprites				Render::m_sSprites;
	Render::LightEnviroment		Render::m_sLightEnviroment;
	/* Buffers */
	Shared<UniformBuffer>		Render::m_sCameraUniformBuffer;
	Shared<UniformBuffer>		Render::m_sClippingUniformBuffer;
	Shared<ShaderStorageBuffer> Render::m_sDirectLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sPointLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sSpotLightsBuffer;

}

void shade::Render::PProcess::Process(const Shared<PostProcess>& pp)
{
	pp->Process();
}

void shade::Render::Init()
{
	if (!m_sIsInit)
	{
		m_sRenderAPI->Init();
		m_sIsInit = true;

		/* Create buffers and other */
		m_sCameraUniformBuffer		= UniformBuffer::Create(sizeof(Camera::RenderData),	0);
		m_sClippingUniformBuffer	= UniformBuffer::Create(sizeof(glm::vec4),			1);
		m_sDirectLightsBuffer		= ShaderStorageBuffer::Create(0,  2);
		m_sPointLightsBuffer		= ShaderStorageBuffer::Create(0,  3);
		m_sSpotLightsBuffer			= ShaderStorageBuffer::Create(0,  4);


		glm::fvec2 plane[4] = { glm::fvec2(-1.0,  1.0 ), glm::fvec2(-1.0, -1.0) ,glm::fvec2(1.0,   1.0), glm::fvec2(1.0,  -1.0) };

	
		m_sSprites.VAO = VertexArray::Create();
		m_sSprites.VBO = VertexBuffer::Create(plane, sizeof(glm::fvec2) * 4, VertexBuffer::BufferType::Static);
		m_sSprites.EBO = VertexBuffer::Create(sizeof(glm::fvec2) * 4, VertexBuffer::BufferType::Dynamic);

		m_sSprites.VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float2,		 "Position"	}});
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
	m_sSubmitedPool.clear();
	m_sInstancePool.clear();

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

void shade::Render::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	m_sRenderAPI->SetViewPort(x, y, width, height);
}

void shade::Render::Begin(Shared<FrameBuffer> framebuffer)
{
	/* Instances */
	for (auto shader = m_sInstancePool.begin(); shader != m_sInstancePool.end();)
	{
		for (auto pool = shader->second.begin(); pool != shader->second.end();)
		{
			if (pool->second.Expired)
				pool = shader->second.erase(pool++);
			else
				++pool;
		}

		if (shader->second.size() == 0)
			m_sInstancePool.erase(shader++);
		else
			++shader;
	}

	/* Submited */
	for (auto shader = m_sSubmitedPool.begin(); shader != m_sSubmitedPool.end();)
	{
		for (auto pool = shader->second.begin(); pool != shader->second.end();)
		{
			if (pool->second.Expired)
				pool = shader->second.erase(pool++);
			else
				++pool;
		}

		if (shader->second.size() == 0)
			m_sSubmitedPool.erase(shader++);
		else
			++shader;
	}

	m_sRenderAPI->Begin(framebuffer);
}

void shade::Render::End(Shared<FrameBuffer> framebuffer)
{
	m_sRenderAPI->End(framebuffer);
	m_sLightEnviroment.Clear();
}

void shade::Render::BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env, const std::size_t& envCount, const glm::vec4& clipping)
{
	//m_sRenderAPI->BeginScene(camera, env, envCount);
	/* Set data to uniforsm buffetrs */
	m_sCameraUniformBuffer->SetData(&camera->GetRenderData(), sizeof(Camera::RenderData), 0);
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

void shade::Render::EndScene()
{
	m_sRenderAPI->EndScene();
}

void shade::Render::SubmitInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	auto& _shaderPool = m_sInstancePool.find(shader.get());
	// If new shader encounter
	if(_shaderPool == m_sInstancePool.end())
	{	// If new drawable encounter
		_CreateInstances(shader, drawable, material, transform);
	}
	else
	{
		auto& _drawablePool = _shaderPool->second.find(drawable);
		if (_drawablePool == _shaderPool->second.end())
		{
			// Create new
			_CreateInstances(shader, drawable, material, transform);
		}
		else
		{
			// Update existing 
			m_sInstancePool[shader.get()][drawable].Expired = false; 
			m_sInstancePool[shader.get()][drawable].Count++;
			m_sInstancePool[shader.get()][drawable].Material = material;
			m_sInstancePool[shader.get()][drawable].Transforms.push_back(transform);
		}
	}
}

void shade::Render::Submit(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	// WE CAN USE m_sInstancePool[shader.get()] and didnt use find for shader only!!!!!!!!!
	auto& _shaderPool = m_sSubmitedPool.find(shader.get());
	// If new shader encounter
	if (_shaderPool == m_sSubmitedPool.end())
	{	// If new drawable encounter
		_CreateInstance(shader, drawable, material, transform);
	}
	else
	{
		auto _drawablePool = _shaderPool->second.find(drawable);
		if (_drawablePool == _shaderPool->second.end())
		{
			// Create new
			_CreateInstance(shader, drawable, material, transform);
		}
		else
		{
			// Update existing 
			_drawablePool->second.Expired = false;
			_drawablePool->second.MaterialTransforms.push_back(std::make_tuple(transform, material));
		}
	}
}

void shade::Render::Submit(const Shared<DirectLight>& light, const glm::mat4& transform)
{
	/* Getting direction */
	m_sLightEnviroment.DirectLightSources.push_back(light->GetRenderData(-glm::normalize(glm::mat3(transform) * glm::vec3(1.0f))));
}

void shade::Render::Submit(const Shared<PointLight>& light, const glm::mat4& transform)
{
	m_sLightEnviroment.PointLightSources.push_back(light->GetRenderData(Transform3D::GetTransformFromMatrix(transform).GetPosition()));
}

void shade::Render::Submit(const Shared<SpotLight>& light, const glm::mat4& transform)
{
	/* Getting position and direction */
	m_sLightEnviroment.SpotLightSources.push_back(light->GetRenderData(Transform3D::GetTransformFromMatrix(transform).GetPosition(), -glm::normalize(glm::mat3(transform) * glm::vec3(1.0f))));
}

void shade::Render::Submit(const Shared<Shader>& shader, const Shared<Texture>& texture, const glm::mat4& transform)
{
	
}

void shade::Render::DrawInstances(const Shared<Shader>& shader)
{
	auto& _shader = m_sInstancePool.find(shader.get());
	if (_shader != m_sInstancePool.end())
	{
		shader->Bind();
		
		shader->ExecuteSubrutines(); // THERE PROBABLU ISSUE 

		for (auto& instance : _shader->second)
		{
			// If size has been changed then need to resize
			if (instance.second.EBO->GetSize() != sizeof(glm::mat4) * instance.second.Transforms.size())
				instance.second.EBO->Resize(sizeof(glm::mat4) * instance.second.Count);

			instance.second.EBO->SetData(instance.second.Transforms.data(), sizeof(glm::mat4) * instance.second.Count, 0);

			/* If material exist */
			if (instance.second.Material)
			{
				/* Send material */
				shader->SendFloat3("u_Material.AmbientColor", glm::value_ptr(instance.second.Material->ColorAmbient));
				shader->SendFloat3("u_Material.DiffuseColor", glm::value_ptr(instance.second.Material->ColorDiffuse));
				shader->SendFloat3("u_Material.SpecularColor", glm::value_ptr(instance.second.Material->ColorSpecular));
				shader->SendFloat3("u_Material.TransparentColor", glm::value_ptr(instance.second.Material->ColorTransparent));
				shader->SendFloat("u_Material.Emissive", instance.second.Material->Emmisive);
				shader->SendFloat("u_Material.Shinines", instance.second.Material->Shininess);
				shader->SendFloat("u_Material.ShininesStrength", instance.second.Material->ShininessStrength);
				shader->SendFloat("u_Material.Opacity", instance.second.Material->Opacity);
				shader->SendFloat("u_Material.Refracti", instance.second.Material->Refracti);

				/*TODO select subrutine depends on materail*/

				/* Attach texures to their slots*/
				if (instance.second.Material->TextureDiffuse)
					instance.second.Material->TextureDiffuse->Bind(0);
				if (instance.second.Material->TextureSpecular)
					instance.second.Material->TextureSpecular->Bind(1);
				if (instance.second.Material->TextureNormals)
					instance.second.Material->TextureNormals->Bind(2);

			}
			/*Execute draw call*/
			m_sRenderAPI->DrawInstanced(instance.second.DrawMode, instance.second.VAO, instance.second.VAO->GetIndexBuffer(), instance.second.Count);
			/* Make as expired */
			instance.second.Expired = true;
			instance.second.Count	= 0;
			instance.second.Transforms.clear();
		}

		shader->UnBind();
	}
}

void shade::Render::DrawSubmited(const Shared<Shader>& shader)
{
	auto& _shader = m_sSubmitedPool.find(shader.get());
	if (_shader != m_sSubmitedPool.end())
	{
		shader->Bind();
	
		shader->ExecuteSubrutines(); // Нужно апдейтить только тогда когда выбрали модель освещения итд

		for (auto& instance : _shader->second)
		{
			for (auto i = 0; i < instance.second.MaterialTransforms.size(); i++)
			{
				shader->SendMat4("u_Transform", false, glm::value_ptr(std::get<0>(instance.second.MaterialTransforms[i])));
			
				auto material = std::get<1>(instance.second.MaterialTransforms[i]).get();
				/* If material exist */
				if (std::get<1>(instance.second.MaterialTransforms[i]))
				{
					/* Send material */
					shader->SendFloat3("u_Material.AmbientColor",		glm::value_ptr(material->ColorAmbient));
					shader->SendFloat3("u_Material.DiffuseColor",		glm::value_ptr(material->ColorDiffuse));
					shader->SendFloat3("u_Material.SpecularColor",		glm::value_ptr(material->ColorSpecular));
					shader->SendFloat3("u_Material.TransparentColor",	glm::value_ptr(material->ColorTransparent));
					shader->SendFloat("u_Material.Emissiv",			    material->Emmisive);
					shader->SendFloat("u_Material.Shinines",			material->Shininess);
					shader->SendFloat("u_Material.ShininesStrength",	material->ShininessStrength);
					shader->SendFloat("u_Material.Opacity",				material->Opacity);
					shader->SendFloat("u_Material.Refracti",			material->Refracti);

					/*TODO select subrutine depends on materail*/

					/* Attach texures to their slots*/
					if (material->TextureDiffuse)
						material->TextureDiffuse->Bind(0);
					if (material->TextureSpecular)
						material->TextureSpecular->Bind(1);
					if (material->TextureNormals)
						material->TextureNormals->Bind(2);

				}

				/*Execute draw call*/
				m_sRenderAPI->DrawIndexed(instance.second.DrawMode, instance.second.VAO, instance.second.VAO->GetIndexBuffer());
				/* Make as expired */
				instance.second.Expired = true;
				instance.second.MaterialTransforms.clear();
			}
		}

		shader->UnBind(); // Need here to successful remove shader from pool
	}
}

void shade::Render::DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO)
{
	m_sRenderAPI->DrawIndexed(mode, VAO, IBO);
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

void shade::Render::DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)
{
	m_sRenderAPI->DrawNotIndexed(mode, VAO, count);
}


void shade::Render::_CreateInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	/* In future - check if it has an animation, and add aditional elemnts in EBO for current drawable*/
	Render::Instance instance;
	instance.DrawMode = drawable->GetDrawMode();
	instance.MaterialTransforms.push_back(std::make_tuple(transform,material));
	instance.VAO = VertexArray::Create();																		/* Im not sure if we use dynamic funtional*/
	instance.VBO = VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Dynamic);
	instance.IBO = IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());
	instance.VBO->SetLayout({   {shade::VertexBuffer::ElementType::Float3, "Position"	},
								{shade::VertexBuffer::ElementType::Float2, "UV_Coords"	},
								{shade::VertexBuffer::ElementType::Float3, "Normal"		},
								{shade::VertexBuffer::ElementType::Float3, "Tangent"	} });
	/* Add all buffers to VAO */
	instance.VAO->AddVertexBuffer(instance.VBO);
	instance.VAO->SetIndexBuffer(instance.IBO);
	instance.Expired = false;
	/* Add new instance */

	//m_sSubmitedPool.insert({ shader.get(),});
	m_sSubmitedPool[shader.get()][drawable] = std::move(instance);
}

void shade::Render::_CreateInstances(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	/* In future - check if it has an animation, and add aditional elemnts in EBO for current drawable*/
	Render::Instances instance;
	instance.Count++;
	instance.DrawMode   = drawable->GetDrawMode();
	instance.Material	= material;
	instance.VAO		= VertexArray::Create();																		/* Im not sure if we use dynamic funtional*/
	instance.VBO		= VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Dynamic);
	instance.EBO		= VertexBuffer::Create(0, VertexBuffer::BufferType::Dynamic);
	instance.IBO		= IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());
	instance.VBO->SetLayout({   {shade::VertexBuffer::ElementType::Float3, "Position"	},
								{shade::VertexBuffer::ElementType::Float2, "UV_Coords"	},
								{shade::VertexBuffer::ElementType::Float3, "Normal"		},
								{shade::VertexBuffer::ElementType::Float3, "Tangent"	} });
	instance.EBO->SetLayout({   {shade::VertexBuffer::ElementType::Mat4,   "Transform"	} });
	/* Add all buffers to VAO */
	instance.VAO->AddVertexBuffer(instance.VBO);
	instance.VAO->AddVertexBuffer(instance.EBO);
	instance.VAO->SetIndexBuffer(instance.IBO);
	instance.Expired = false;
	instance.Transforms.push_back(transform);
	/* Add new instance */
	m_sInstancePool[shader.get()][drawable] = std::move(instance);
}