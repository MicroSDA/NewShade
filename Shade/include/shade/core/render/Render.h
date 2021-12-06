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
#include "shade/core/mesh/Material3D.h"
#include "shade/core/mesh/Mesh.h"
#include "shade/core/render/drawable/primitives/Grid.h"
#include "shade/core/render/drawable/primitives/Box.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/render/postprocess/PPBloom.h"
#include "shade/core/render/postprocess/PPColorCorrection.h"
#include "shade/core/render/drawable/primitives/Plane.h"
#include "shade/core/vertex/Vertex2D.h"

namespace shade
{
	class SHADE_API Render
	{
	public:
		struct Sprites
		{
			Shared<VertexArray>				VAO;
			Shared<VertexBuffer>			VBO;
			Shared<VertexBuffer>			EBO;
			Shared<IndexBuffer>				IBO;
			std::vector<std::tuple<glm::mat4, Shared<Texture>>>	TexturesTransforms;
		};
		struct Instance
		{
			Drawable::DrawMode				DrawMode;
			Shared<VertexArray>				VAO;
			Shared<VertexBuffer>			VBO;
			Shared<IndexBuffer>				IBO;
			std::vector<std::tuple<glm::mat4, Shared<Material3D>>> MaterialTransforms;
			bool							Expired = true;
		};
		struct Instances
		{
			Drawable::DrawMode          DrawMode;
			std::uint32_t               Count = 0;
			std::vector<glm::mat4>		Transforms;
			Shared<Material3D>			Material;
			Shared<VertexArray>         VAO;
			Shared<VertexBuffer>		VBO;
			Shared<VertexBuffer>		EBO;
			Shared<IndexBuffer>			IBO;
			bool						Expired = true;
		};
		using InstancePool = std::unordered_map<Shader* ,std::unordered_map<shade::Shared<Drawable>, Instances>>;
		using SubmitedPool = std::unordered_map<Shader* ,std::unordered_map<shade::Shared<Drawable>, Instance>>;
	public:
		class SHADE_API PProcess
		{
		public:
			static void Process(const Shared<PostProcess>& pp);
		};
	public:
		static void Init();
		static void ShutDown();
		static unsigned int GetVideoMemoryUsage();
		static void SetClearColor(const float& r, const float& g, const float& b,const float& a);
		static void Clear();
		static void DepthTest(const bool& enable);
		static void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);

		static void Begin(Shared<FrameBuffer> framebuffer = nullptr);
		static void End(Shared<FrameBuffer> framebuffer = nullptr);
		static void BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env = nullptr, const std::size_t& envCount = 0, const glm::vec4& clipping = glm::vec4(0));
		static void EndScene();

		static void SubmitInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform);
		static void Submit(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform);

		/* Has to be reworked */
		static void Submit(const Shared<Shader>& shader, const Shared<Texture>&  texture,  const glm::mat4& transform);

		static void DrawInstances(const Shared<Shader>& shader);
		static void DrawSubmited(const Shared<Shader>& shader);


		static void DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count);
		static void DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO);
		static void DrawSprite(const Shared<Shader>& shader, const Shared<Texture>& texture, const glm::mat4& transform, const glm::vec4& rectangle = glm::vec4(0, 0, 1, 1));

	private:
		
		static Unique<RenderAPI> m_sRenderAPI;
		static bool				m_sIsInit;

		static Render::InstancePool   m_sInstancePool;
		static Render::SubmitedPool   m_sSubmitedPool;
		static Render::Sprites		  m_sSprites;

		static Shared<UniformBuffer> m_sCameraUniformBuffer;
		static Shared<UniformBuffer> m_sClippingUniformBuffer;

		static Shared<ShaderStorageBuffer>	m_sDirectLightsBuffer;
		static Shared<ShaderStorageBuffer>	m_sPointLightsBuffer;
		static Shared<ShaderStorageBuffer>	m_sSpotLightsBuffer;
		//Util
		static void _CreateInstance(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform);
		static void _CreateInstances(const Shared<Shader>& shader, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform);

	};
}