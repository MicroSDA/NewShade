#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API DirectLight : public shade::Light
	{
	public:
		struct RenderData
		{
			alignas(16) glm::vec3 Direction;
			alignas(16) glm::vec3 AmbientColor;
			alignas(16) glm::vec3 DiffuseColor;
			alignas(16) glm::vec3 SpecularColor;
			alignas(16) glm::vec3 Position;
			alignas(16)	glm::mat4 ViewMatrix;
		};
	public:
		DirectLight();
		virtual ~DirectLight();
		static std::uint32_t GetTotalCount();
		RenderData GetRenderData(const glm::vec3& direction);
	private:	
		// Total count of created direct lights
		static std::uint32_t m_TotalCount;
	};

}


