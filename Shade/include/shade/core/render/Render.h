#pragma once
#include "shade/core/render/RenderAPI.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/buffers/VertexArray.h"
#include "shade/core/environment/lighting/DirectLight.h"
#include "shade/core/environment/lighting/PointLight.h"
#include "shade/core/environment/lighting/SpotLight.h"
#include "shade/core/render/Shader.h"
#include "shade/core/render/drawable/Drawable.h"
#include "shade/core/environment/Environment.h"
#include "shade/core/image/Texture.h"
#include "shade/core/mesh/Material.h"
#include "shade/core/mesh/Mesh.h"
#include "shade/core/render/drawable/primitives/Grid.h"
#include "shade/core/render/buffers/UniformBuffer.h"

namespace shade
{
	class SHADE_API Render
	{
	public:
		struct InstancedRender
		{
			Drawable::DrawMode                  DrawMode;
			Shared<VertexArray>					VAO;
			const Material*						Material;
			const std::vector<Shared<Texture>>* Textures;
			std::uint32_t						InstanceCount = 0;
			std::vector<glm::mat4>				Transforms;
			bool                                IsExpired = true;
		};
		struct Submited
		{
			Drawable::DrawMode                  DrawMode;
			Shared<VertexArray>					VAO;
			const Material* Material;
			const std::vector<Shared<Texture>>* Textures;
			std::vector<glm::mat4>				Transforms;
			bool                                IsExpired = true;
		};
	public:
		static void Init();
		static void SetClearColor(const float& r, const float& g, const float& b,const float& a);
		static void Clear();
		static void DepthTest(const bool& enable);
		static void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);
		static void Submit(const Drawable* drawable, const glm::mat4& transform, const Material* material = nullptr, const std::vector<Shared<Texture>>* textures = nullptr);
		static void DrawSubmited(const Shared<Shader>& shader);
		static void SubmitInstanced(const Drawable* drawable, const glm::mat4& transform, const Material& material, const std::vector<Shared<Texture>>& textures, const std::uint32_t& count = 1);
		static void DrawInstanced(const Shared<Shader>& shader);
		static void DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO);
		static void Begin(Shared<FrameBuffer> framebuffer = nullptr);
		static void BeginScene(
			const Shared<Shader>& shader,
			const Shared<Camera>& camera,
			const Shared<Environment>* enviroments = nullptr,
			const std::size_t& enviromentsCount = 0, const glm::vec4& clipping = glm::vec4(0));
		static void EndScene(const Shared<Shader>& shader);
		static void End(Shared<FrameBuffer> framebuffer = nullptr);
		static void DrawIndexed(const Drawable* drawable, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO);
	private:
		static Unique<RenderAPI>	m_sRenderAPI;
		static bool					m_sIsInit;

		static std::unordered_map<const Drawable*, InstancedRender> m_sInstancedRender;
		static std::unordered_map<const Drawable*, Submited>		m_sSubmitedRender;
		static Shared<UniformBuffer>								m_sCameraUniformBuffer;
		static Shared<UniformBuffer>								m_sClippingUniformBuffer;
	};
}