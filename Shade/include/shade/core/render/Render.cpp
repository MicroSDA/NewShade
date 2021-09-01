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
	/* Buffers */
	Shared<UniformBuffer>		Render::m_sCameraUniformBuffer;
	Shared<UniformBuffer>		Render::m_sClippingUniformBuffer;
	Shared<ShaderStorageBuffer> Render::m_sDirectLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sPointLightsBuffeer;
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
		m_sPointLightsBuffeer		= ShaderStorageBuffer::Create(0,  3);
		m_sSpotLightsBuffer			= ShaderStorageBuffer::Create(0,  4);

	}
	else
		SHADE_CORE_WARNING("Render API has been already initialized!");
}

void shade::Render::ShutDown()
{
	m_sRenderAPI->ShutDown();
	m_sSubmitedPool.clear();
	m_sInstancePool.clear();
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
	
	m_sRenderAPI->Begin(framebuffer);
	/* Instances */
	for (auto shader = m_sInstancePool.begin(); shader != m_sInstancePool.end();)
	{
		auto& instancesPool = shader->second.begin();
		while (instancesPool != shader->second.end())
		{
			if (instancesPool->second.Expired)
				instancesPool = shader->second.erase(instancesPool++);
			else
				++instancesPool;
		}

		if (shader->second.size() == 0)
			m_sInstancePool.erase(shader++);
		else
			++shader;
	}
	/* Submited */
	for (auto shader = m_sSubmitedPool.begin(); shader != m_sSubmitedPool.end();)
	{
		auto& submitedPool = shader->second.begin();
		while (submitedPool != shader->second.end())
		{
			if (submitedPool->second.Expired)
				submitedPool = shader->second.erase(submitedPool++);
			else
				++submitedPool;
		}

		if (shader->second.size() == 0)
			shader = m_sSubmitedPool.erase(shader++);
		else
			++shader;
	}
}

void shade::Render::End(Shared<FrameBuffer> framebuffer)
{
	m_sRenderAPI->End(framebuffer);
	/* Instances */
	for (auto& shader : m_sInstancePool)
	{
		for (auto& instances : shader.second)
		{
			if (!instances.second.Expired)
			{
				instances.second.Count = 0;
				instances.second.Transforms.clear();
				instances.second.Expired = true;
			}
		}
	}
	/* Submited */
	for (auto& shader : m_sSubmitedPool)
	{
		for (auto& instances : shader.second)
		{
			if (!instances.second.Expired)
			{
				instances.second.MaterialTransforms.clear();
				instances.second.Expired = true;
			}
		}
	}
}

void shade::Render::BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env, const std::size_t& envCount, const glm::vec4& clipping)
{

	m_sRenderAPI->BeginScene(camera, env, envCount);
	/* Set data to uniforsm buffetrs */
	m_sCameraUniformBuffer->SetData(&camera->GetData(), sizeof(Camera::RenderData), 0);
	m_sClippingUniformBuffer->SetData(glm::value_ptr(clipping), sizeof(glm::vec4), 0);

	/* Process SSBO buffers */
	std::uint32_t dLightIndex = 0, pLighIndex = 0, sLightIndex = 0;

	if (m_sDirectLightsBuffer->GetSize() != sizeof(DirectLight::RenderData) * DirectLight::GetTotalCount())
		m_sDirectLightsBuffer->Resize(sizeof(DirectLight::RenderData) * DirectLight::GetTotalCount());
	if (m_sPointLightsBuffeer->GetSize() != sizeof(PointLight::RenderData) * PointLight::GetTotalCount())
		m_sPointLightsBuffeer->Resize(sizeof(PointLight::RenderData) * PointLight::GetTotalCount());
	if (m_sSpotLightsBuffer->GetSize() != sizeof(SpotLight::RenderData) * SpotLight::GetTotalCount())
		m_sSpotLightsBuffer->Resize(sizeof(SpotLight::RenderData) * SpotLight::GetTotalCount());

	for (auto i = 0; i < envCount; i++)
	{
		switch (env[i]->GetType())
		{
		case Environment::Type::DirectLight:
			m_sDirectLightsBuffer->SetData(env[i]->ReciveRenderData<DirectLight>(), sizeof(DirectLight::RenderData), sizeof(DirectLight::RenderData) * dLightIndex);
			dLightIndex++;
			break;
		case Environment::Type::PointLight:
			m_sPointLightsBuffeer->SetData(env[i]->ReciveRenderData<PointLight>(), sizeof(PointLight::RenderData), sizeof(PointLight::RenderData) * pLighIndex);
			pLighIndex++;
			break;
		case Environment::Type::SpotLight:
			m_sSpotLightsBuffer->SetData(env[i]->ReciveRenderData<SpotLight>(), sizeof(SpotLight::RenderData), sizeof(SpotLight::RenderData) * sLightIndex);
			sLightIndex++;
			break;
		}

	}
	/*!Process SSBO buffers*/
}

void shade::Render::EndScene()
{
	m_sRenderAPI->EndScene();
}

void shade::Render::SubmitInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	// WE CAN USE m_sInstancePool[shader.get()] and didnt use find for shader only!!!!!!!!!
	auto& _shaderPool = m_sInstancePool.find(shader.get());
	// If new shader encounter
	if(_shaderPool == m_sInstancePool.end())
	{	// If new drawable encounter
		_CreateInstances(shader, drawable, material, transform);
	}
	else
	{
		auto& _drawablePool = _shaderPool->second.find(drawable.get());
		if (_drawablePool == _shaderPool->second.end())
		{
			// Create new
			_CreateInstances(shader, drawable, material, transform);
		}
		else
		{
			// Update existing 
			m_sInstancePool[shader.get()][drawable.get()].Expired = false;
			m_sInstancePool[shader.get()][drawable.get()].Count++;
			m_sInstancePool[shader.get()][drawable.get()].Material = material;
			m_sInstancePool[shader.get()][drawable.get()].Transforms.push_back(transform);
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
		auto& _drawablePool = _shaderPool->second.find(drawable.get());
		if (_drawablePool == _shaderPool->second.end())
		{
			// Create new
			_CreateInstance(shader, drawable, material, transform);
		}
		else
		{
			// Update existing 
			m_sSubmitedPool[shader.get()][drawable.get()].Expired = false;
			m_sSubmitedPool[shader.get()][drawable.get()].MaterialTransforms.push_back(std::make_tuple(transform, material));
		}
	}
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
				shader->SendFlaot3("u_Material.AmbientColor", glm::value_ptr(instance.second.Material->ColorAmbient));
				shader->SendFlaot3("u_Material.DiffuseColor", glm::value_ptr(instance.second.Material->ColorDiffuse));
				shader->SendFlaot3("u_Material.SpecularColor", glm::value_ptr(instance.second.Material->ColorSpecular));
				shader->SendFlaot3("u_Material.TransparentColor", glm::value_ptr(instance.second.Material->ColorTransparent));
				shader->SendFlaot("u_Material.Shinines", instance.second.Material->Shininess);
				shader->SendFlaot("u_Material.ShininesStrength", instance.second.Material->ShininessStrength);
				shader->SendFlaot("u_Material.Opacity", instance.second.Material->Opacity);
				shader->SendFlaot("u_Material.Refracti", instance.second.Material->Refracti);

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
		}
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
					shader->SendFlaot3("u_Material.AmbientColor",		glm::value_ptr(material->ColorAmbient));
					shader->SendFlaot3("u_Material.DiffuseColor",		glm::value_ptr(material->ColorDiffuse));
					shader->SendFlaot3("u_Material.SpecularColor",		glm::value_ptr(material->ColorSpecular));
					shader->SendFlaot3("u_Material.TransparentColor",	glm::value_ptr(material->ColorTransparent));
					shader->SendFlaot("u_Material.Shinines",			material->Shininess);
					shader->SendFlaot("u_Material.ShininesStrength",	material->ShininessStrength);
					shader->SendFlaot("u_Material.Opacity",				material->Opacity);
					shader->SendFlaot("u_Material.Refracti",			material->Refracti);

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
			}
		}
	}
}

void shade::Render::DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO)
{
	m_sRenderAPI->DrawIndexed(mode, VAO, IBO);
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
	instance.DrawMode = drawable->GetDrawMode();
	instance.VAO = VertexArray::Create();																		/* Im not sure if we use dynamic funtional*/
	instance.VBO = VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Dynamic);
	instance.IBO = IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());
	instance.VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float3,   "Position"	},
								{shade::VertexBuffer::ElementType::Float2, "UV_Coords"	},
								{shade::VertexBuffer::ElementType::Float3, "Normal"		},
								{shade::VertexBuffer::ElementType::Float3, "Tangent"	} });
	/* Add all buffers to VAO */
	instance.VAO->AddVertexBuffer(instance.VBO);
	instance.VAO->SetIndexBuffer(instance.IBO);
	/* Add new instance */
	m_sSubmitedPool[shader.get()][drawable.get()] = instance;
}

void shade::Render::_CreateInstances(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform)
{
	/* In future - check if it has an animation, and add aditional elemnts in EBO for current drawable*/
	Render::Instances instance;
	instance.Count++;
	instance.DrawMode   = drawable->GetDrawMode();
	instance.Material	= material;
	instance.DrawMode	= drawable->GetDrawMode();
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
	instance.Transforms.push_back(transform);
	/* Add new instance */
	m_sInstancePool[shader.get()][drawable.get()] = instance;
}

//void shade::Render::Bloom(const Shared<FrameBuffer>& inputFrameBuffer, const Shared<FrameBuffer>& outFrameBuffer, const Shared<Shader>& bloomShader)
//{
//	m_sRenderAPI->Bloom(inputFrameBuffer, outFrameBuffer, bloomShader);
//}