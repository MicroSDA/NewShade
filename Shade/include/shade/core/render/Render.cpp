#include "shade_pch.h"
#include "Render.h"
#include "shade/core/vertex/Vertex3D.h"

namespace shade
{
	Unique<RenderAPI> Render::m_sRenderAPI = RenderAPI::Create();

	bool Render::m_sIsInit = false;
	std::unordered_map<const Drawable*, Render::InstancedRender> Render::m_sInstancedRender;
	std::unordered_map<const Drawable*, Render::Submited> Render::m_sSubmitedRender;
	Shared<UniformBuffer> Render::m_sCameraUniformBuffer;
	Shared<UniformBuffer> Render::m_sClippingUniformBuffer;
	Shared<ShaderStorageBuffer> Render::m_sTestShaderStorageBuffer;

	struct LTest
	{
		glm::vec3 Direction = glm::vec3(1, 0, 0);
		float x;
		glm::vec3 Ambient	= glm::vec3(1 ,1 ,0);
		float y;
		glm::vec3 Diffuse   = glm::vec3(1, 1, 1);
		float z;
		glm::vec3 Specular	= glm::vec3(0, 1, 1);
		float w;
	
	};
}

void shade::Render::Init()
{
	if (!m_sIsInit)
	{
		m_sRenderAPI->Init();
		m_sIsInit = true;


		m_sCameraUniformBuffer = UniformBuffer::Create(sizeof(Camera::Data), 0);
		m_sClippingUniformBuffer = UniformBuffer::Create(sizeof(glm::vec4), 1);
		m_sTestShaderStorageBuffer = ShaderStorageBuffer::Create(sizeof(LTest) * 2, 2);
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

void shade::Render::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	m_sRenderAPI->SetViewPort(x, y, width, height);
}
void shade::Render::Begin(Shared<FrameBuffer> framebuffer)
{
	m_sRenderAPI->Begin(framebuffer);
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
	m_sRenderAPI->End(framebuffer);

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
	shader->Bind();
	m_sCameraUniformBuffer->SetData(&camera->GetData(), sizeof(Camera::Data), 0);
	m_sClippingUniformBuffer->SetData(glm::value_ptr(clipping), sizeof(glm::vec4), 0);
	int test = 1+ rand() % 10;

	LTest lTes[2];
	lTes[1].Diffuse = glm::vec3(0, 0.5, 1);
	m_sTestShaderStorageBuffer->SetData(lTes, sizeof(LTest) * 2, 0);
	m_sRenderAPI->BeginScene(shader, camera, enviroments, enviromentsCount);
}

void shade::Render::EndScene(const Shared<Shader>& shader)
{
	m_sRenderAPI->EndScene(shader);
}

