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
		};
	public:
		DirectLight();
		virtual ~DirectLight();
		void SetDirection(const float& x, const float& y, const float& z);
		void SetDirection(const glm::fvec3& direction);
		const glm::fvec3& GetDirection() const;
		glm::fvec3& GetDirection();

		static std::uint32_t GetTotalCount();
		RenderData GetRenderData();
	protected:
		glm::fvec3 m_Direction;
	private:	
		// Total count of created direct lights
		static std::uint32_t m_TotalCount;
	};

}


