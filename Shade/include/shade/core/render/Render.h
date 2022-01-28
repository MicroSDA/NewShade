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
#include "shade/core/render/drawable/primitives/Sphere.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include "shade/core/render/buffers/ShaderStorageBuffer.h"
#include "shade/core/render/postprocess/PPBloom.h"
#include "shade/core/render/postprocess/PPColorCorrection.h"
#include "shade/core/render/drawable/primitives/Plane.h"
#include "shade/core/vertex/Vertex2D.h"
#include "shade/core/render/DrawablePass.h"
#include "shade/core/render/RenderPipeline.h"

namespace shade
{
	

	class SHADE_API Render
	{
	public:
		struct LightEnviroment
		{
			std::vector<DirectLight::RenderData> DirectLightSources;
			std::vector<PointLight::RenderData>  PointLightSources;
			std::vector<SpotLight::RenderData>   SpotLightSources;
			void Clear()
			{
				DirectLightSources.clear();
				PointLightSources.clear();
				SpotLightSources.clear();
			}
		};

		struct Sprites
		{
			Shared<VertexArray>				VAO;
			Shared<VertexBuffer>			VBO;
			Shared<VertexBuffer>			EBO;
			Shared<IndexBuffer>				IBO;
			std::vector<std::tuple<glm::mat4, Shared<Texture>>>	TexturesTransforms;
		};

		struct RenderPipelines
		{
			std::unordered_map<std::shared_ptr<RenderPipeline>, DrawablePools> Pipelines;
		};

	public:
		class SHADE_API PProcess
		{
		public:
			static void Process(const Shared<PostProcess>& pp);
		};
	public:
		static Unique<RenderAPI>& GetRenderApi();
		static void Init();
		static void ShutDown();
		static unsigned int GetVideoMemoryUsage();
		static void SetClearColor(const float& r, const float& g, const float& b,const float& a);
		static void Clear();
		static void DepthTest(const bool& enable);
		static void CullFace(const int& mode);// Test
		static void SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height);

		static void Begin();
		static void End();
		static void BeginScene(const Shared<Camera>& camera, const Shared<FrameBuffer>& framebuffer, const glm::vec4& clipping = glm::vec4(0));
		static void EndScene();
		/* Light */
		static void SubmitLight(const Shared<DirectLight>& light,  const glm::mat4& transform);
		static void SubmitLight(const Shared<PointLight>&  light,  const glm::mat4& transform);
		static void SubmitLight(const Shared<SpotLight>&   light,  const glm::mat4& transform);

		static void DrawSprite(const Shared<Shader>& shader, const Shared<Texture>& texture, const glm::mat4& transform, const glm::vec4& rectangle = glm::vec4(0, 0, 1, 1));
		static void DrawSprite(const Shared<Shader>& shader, const std::uint32_t&   texture, const glm::mat4& transform, const glm::vec4& rectangle = glm::vec4(0, 0, 1, 1));

		static void SubmitPipelineInstanced(const Shared<RenderPipeline>& pipeline, const Shared<Drawable>& drawable, const Shared<Material3D>& material, const glm::mat4& transform);

		static void ExecuteSubmitedPipeline(const Shared<RenderPipeline>& pipeline);
		static void ExecuteSubmitedPipelines();

		static void Enable(const int& value);
		static void Disable(const int& value);

		static const LightEnviroment& GetSubmitedLight();
		static const Shared<Camera>&  GetLastActiveCamera();
	private:
		
		static Unique<RenderAPI> m_sRenderAPI;
		static bool				 m_sIsInit;

		static Render::Sprites				m_sSprites;
		static Render::LightEnviroment		m_sLightEnviroment;

		static Shared<UniformBuffer>		m_sCameraUniformBuffer;
		static Shared<UniformBuffer>		m_sClippingUniformBuffer;

		/* At scene begin framebuffer */
		static Shared<FrameBuffer>			m_sTargetFrameBuffer;
		/* Ping pong frame buffer */
		static Shared<FrameBuffer>			m_sPreviousPassBuffer;
		static Shared<RenderPipeline>		m_sPreviousPipeline;

		static Shared<ShaderStorageBuffer>	m_sDirectLightsBuffer;
		static Shared<ShaderStorageBuffer>	m_sPointLightsBuffer;
		static Shared<ShaderStorageBuffer>	m_sSpotLightsBuffer;

		static Shared<Camera>				m_sCamera;

		static std::unordered_map<Shared<Drawable>, BufferDrawData> m_sInstancedGeometryBuffers;
		static RenderPipelines m_sPipelines;
		static void _CreateInstancedGeometryBuffer(const Shared<Drawable>& drawable);
	};
}