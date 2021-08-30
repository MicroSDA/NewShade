#include "shade_pch.h"
#include "Render.h"
#include "shade/core/vertex/Vertex3D.h"

namespace shade
{
	Unique<RenderAPI> Render::m_sRenderAPI = RenderAPI::Create();

	bool Render::m_sIsInit = false;
	std::unordered_map<const Drawable*, Render::InstancedRender> Render::m_sInstancedRender;
	std::unordered_map<const Drawable*, Render::Submited> Render::m_sSubmitedRender;

	Shared<UniformBuffer>		Render::m_sCameraUniformBuffer;
	Shared<UniformBuffer>		Render::m_sClippingUniformBuffer;
	Shared<ShaderStorageBuffer> Render::m_sDirectLightsBuffer;
	Shared<ShaderStorageBuffer> Render::m_sPointLightsBuffeer;
	Shared<ShaderStorageBuffer> Render::m_sSpotLightsBuffer;
	Render::InstancePool		Render::m_sInstancePool;
}

void shade::Render::Init()
{
	if (!m_sIsInit)
	{
		m_sRenderAPI->Init();
		m_sIsInit = true;


		m_sCameraUniformBuffer		= UniformBuffer::Create(sizeof(Camera::RenderData),	0);
		m_sClippingUniformBuffer	= UniformBuffer::Create(sizeof(glm::vec4),			1);
		m_sDirectLightsBuffer		= ShaderStorageBuffer::Create(0,  2);
		m_sPointLightsBuffeer		= ShaderStorageBuffer::Create(0,  3);
		m_sSpotLightsBuffer			= ShaderStorageBuffer::Create(0,  4);

		
	}
	else
		SHADE_CORE_WARNING("Render API has been already initialized!");
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

void shade::Render::SubmitInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const glm::mat4& transform)
{
	// WE CAN USE m_sInstancePool[shader.get()] and didnt use find for shader only!!!!!!!!!
	auto& _shaderPool = m_sInstancePool.find(shader.get());
	// If new shader encounter
	if(_shaderPool == m_sInstancePool.end())
	{	// If new drawable encounter
		_CreateInstance(shader, drawable, transform);
	}
	else
	{
		auto& _drawablePool = _shaderPool->second.find(drawable.get());
		if (_drawablePool == _shaderPool->second.end())
		{
			// Create new
			_CreateInstance(shader, drawable, transform);
		}
		else
		{
			// Update existing 
			m_sInstancePool[shader.get()][drawable.get()].Expired = false;
			m_sInstancePool[shader.get()][drawable.get()].Count++;
			m_sInstancePool[shader.get()][drawable.get()].Transforms.push_back(transform);
		}
	}
}

void shade::Render::DrawInstances(const Shared<Shader>& shader)
{
	// Need to set flag expired and resert transform and etc

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
			//TODO Need bind material in future
			m_sRenderAPI->DrawInstanced(instance.second.DrawMode, instance.second.VAO, instance.second.VAO->GetIndexBuffer(), instance.second.Count);
		}
	}
}

void shade::Render::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	m_sRenderAPI->SetViewPort(x, y, width, height);
}
void shade::Render::Begin(Shared<FrameBuffer> framebuffer)
{
	m_sRenderAPI->Begin(framebuffer);
	/* New test section*/
	{

		for (auto& shader : m_sInstancePool)
		{
			auto& instancesPool = shader.second.begin();
			while (instancesPool != shader.second.end())
			{
				if (instancesPool->second.Expired)
					instancesPool = shader.second.erase(instancesPool);
				else
					instancesPool++;
			}
		}
	}

	auto& instancedPool = m_sInstancedRender.begin();
	while (instancedPool != m_sInstancedRender.end())
	{
		if (instancedPool->second.IsExpired)
			instancedPool = m_sInstancedRender.erase(instancedPool);
		else
		{
			instancedPool++;
		}
	}

	auto& submitedPool = m_sSubmitedRender.begin();

	while (submitedPool != m_sSubmitedRender.end())
	{
		if (submitedPool->second.IsExpired)
			submitedPool = m_sSubmitedRender.erase(submitedPool);
		else
		{
			submitedPool++;
		}
	}

	
}

void shade::Render::End(Shared<FrameBuffer> framebuffer)
{
	// NEED TO REDISING THERE always expired
	m_sRenderAPI->End(framebuffer);

	/* New section here*/
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

	for (auto& instances : m_sInstancedRender)
	{
		if (!instances.second.IsExpired)
		{
			instances.second.InstanceCount = 0;
			instances.second.Transforms.clear();
			instances.second.IsExpired = true;
		}
	}
	for (auto& instance : m_sSubmitedRender)
	{
		if (!instance.second.IsExpired)
		{
			instance.second.IsExpired = true;
			instance.second.Transforms.clear();
		}	
	}
}
void shade::Render::DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO)
{
	m_sRenderAPI->DrawIndexed(mode, VAO, IBO);
}

void shade::Render::_CreateInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const glm::mat4& transform)
{
	/* In future - check if it has an animation, and add aditional elemnts in EBO for current drawable*/
	Render::Instance instance;
	instance.Count++;
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
void shade::Render::Submit(const Drawable* drawable, const glm::mat4& transform, const Material* material, const std::vector<Shared<Texture>>* textures)
{
	auto& instance = m_sSubmitedRender.find(drawable);
	if (instance == m_sSubmitedRender.end())
	{
		Submited submited;
		submited.DrawMode = drawable->GetDrawMode();
		submited.Material = material;
		submited.Transforms.push_back(transform);
		submited.Textures = textures;
		submited.IsExpired = false;

		submited.VAO = VertexArray::Create();
		auto VBO = VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Dynamic);
		VBO->SetLayout({ {shade::VertexBuffer::ElementType::Float3, "Position"},
							{shade::VertexBuffer::ElementType::Float2, "UV_Coords"},
							{shade::VertexBuffer::ElementType::Float3, "Normal"},
							{shade::VertexBuffer::ElementType::Float3, "Tangent"} });
		auto IBO = IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());
		submited.VAO->AddVertexBuffer(VBO);
		submited.VAO->SetIndexBuffer(IBO);

		m_sSubmitedRender[drawable] = submited;
	}
	else if (instance != m_sSubmitedRender.end())
	{
		instance->second.IsExpired = false;
		instance->second.Transforms.push_back(transform);
	}
}

void shade::Render::DrawSubmited(const Shared<Shader>& shader)
{
	shader->Bind();
	for (auto& instance : m_sSubmitedRender)
	{
		if(instance.second.Material != nullptr)
			instance.second.Material->Process(shader);
		
		if (instance.second.Textures != nullptr)
		{
			std::uint32_t textureUnit = 0;
			for (auto& texture : *instance.second.Textures)
			{
				texture->Bind(shader, textureUnit++);
			}
		}
		
		for (auto& transform : instance.second.Transforms)
		{
			shader->SendMat4("Transform", false, glm::value_ptr(transform));
			m_sRenderAPI->DrawIndexed(instance.second.DrawMode, instance.second.VAO, instance.second.VAO->GetIndexBuffer());
		}
		
	}
}

void shade::Render::DrawInstanced(const Shared<Shader>& shader)
{
	for (auto& instance : m_sInstancedRender)
	{
		instance.second.Material->Process(shader);
		std::uint32_t textureUnit = 0;
		for (auto& texture : *instance.second.Textures)
		{
			texture->Bind(shader, textureUnit++);
		}

	
		instance.second.VAO->GetVertexBuffers()[1]->SetData(instance.second.Transforms.data(), sizeof(glm::mat4) * instance.second.InstanceCount, 0);
		m_sRenderAPI->DrawInstanced(instance.second.DrawMode, instance.second.VAO, instance.second.VAO->GetIndexBuffer(), instance.second.InstanceCount);
	}
}

void shade::Render::DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)
{
	m_sRenderAPI->DrawNotIndexed(mode, VAO, count);
}

void shade::Render::SubmitInstanced(const Drawable* drawable, const glm::mat4& transform, const Material& material, const std::vector<Shared<Texture>>& textures)
{
	// Wrong Count !!!!!!! its total enties caount not current instances

	auto& instances = m_sInstancedRender.find(drawable);
	if (instances == m_sInstancedRender.end())
	{
		InstancedRender newInstances;
		newInstances.DrawMode = drawable->GetDrawMode();
		newInstances.Material = &material;
		newInstances.Textures = &textures;
		//newInstances.Transforms.reserve(count);
		newInstances.IsExpired = false;

		newInstances.VAO = VertexArray::Create();
		auto VBO = VertexBuffer::Create(drawable->GetVertices().data(), sizeof(Vertex3D) * drawable->GetVertices().size(), VertexBuffer::BufferType::Dynamic);
		VBO->SetLayout({	{shade::VertexBuffer::ElementType::Float3, "Position"},
							{shade::VertexBuffer::ElementType::Float2, "UV_Coords"},
							{shade::VertexBuffer::ElementType::Float3, "Normal"},
							{shade::VertexBuffer::ElementType::Float3, "Tangent"} });
		// Create empty ebo set data later
		auto EBO = VertexBuffer::Create(sizeof(glm::mat4), VertexBuffer::BufferType::Dynamic); // size is 10 !!!
		EBO->SetLayout({ {shade::VertexBuffer::ElementType::Mat4,	"Transform"} });

		auto IBO = IndexBuffer::Create(drawable->GetIndices().data(), drawable->GetIndices().size());
		newInstances.VAO->AddVertexBuffer(VBO);
		newInstances.VAO->AddVertexBuffer(EBO);
		newInstances.VAO->SetIndexBuffer(IBO);

		newInstances.Transforms.push_back(transform);
		m_sInstancedRender[drawable] = newInstances;
	}
	else if(instances != m_sInstancedRender.end())
	{
		instances->second.Transforms.push_back(transform);
		instances->second.InstanceCount++; // We can get it form transforms.size()
		instances->second.IsExpired = false;

		if (instances->second.VAO->GetVertexBuffers()[1]->GetSize() != sizeof(glm::mat4) * instances->second.Transforms.size())
		{
			instances->second.VAO->GetVertexBuffers()[1]->Resize(sizeof(glm::mat4) * instances->second.Transforms.size());
		}

	}
}

void shade::Render::BeginScene(const Shared<Shader>& shader, const Shared<Camera>& camera, const Shared<Environment>* enviroments, const std::size_t& enviromentsCount, const glm::vec4& clipping)
{
	// ÍÀÄÎ ÏÎÔÈÊ
	
	m_sCameraUniformBuffer->SetData(&camera->GetData(), sizeof(Camera::RenderData), 0);
	m_sClippingUniformBuffer->SetData(glm::value_ptr(clipping), sizeof(glm::vec4),  0);


	std::uint32_t dLightIndex = 0, pLighIndex = 0, sLightIndex = 0;

	if (m_sDirectLightsBuffer->GetSize() != sizeof(DirectLight::RenderData) * DirectLight::GetTotalCount())
		m_sDirectLightsBuffer->Resize(sizeof(DirectLight::RenderData) * DirectLight::GetTotalCount());
	if (m_sPointLightsBuffeer->GetSize() != sizeof(PointLight::RenderData) * PointLight::GetTotalCount())
		m_sPointLightsBuffeer->Resize(sizeof(PointLight::RenderData) * PointLight::GetTotalCount());
	if (m_sSpotLightsBuffer->GetSize() != sizeof(SpotLight::RenderData) * SpotLight::GetTotalCount())
		m_sSpotLightsBuffer->Resize(sizeof(SpotLight::RenderData) * SpotLight::GetTotalCount());


	for (auto i = 0; i < enviromentsCount; i++)
	{
		switch (enviroments[i]->GetType())
		{
		case Environment::Type::DirectLight:
			m_sDirectLightsBuffer->SetData(enviroments[i]->ReciveRenderData<DirectLight>(), sizeof(DirectLight::RenderData), sizeof(DirectLight::RenderData) * dLightIndex);
			dLightIndex++;
			break;
		case Environment::Type::PointLight:
			m_sPointLightsBuffeer->SetData(enviroments[i]->ReciveRenderData<PointLight>(), sizeof(PointLight::RenderData), sizeof(PointLight::RenderData) * pLighIndex);
			pLighIndex++;
			break;
		case Environment::Type::SpotLight:
			m_sSpotLightsBuffer->SetData(enviroments[i]->ReciveRenderData<SpotLight>(),     sizeof(SpotLight::RenderData), sizeof(SpotLight::RenderData) * sLightIndex);
			sLightIndex++;
			break;
		}
		
	}

	//shader->Bind();
	//m_sRenderAPI->BeginScene(shader, camera, enviroments, enviromentsCount);
	
}

void shade::Render::EndScene(const Shared<Shader>& shader)
{
	m_sRenderAPI->EndScene(shader);
}

