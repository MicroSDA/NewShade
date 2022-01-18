#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API PointLight : public shade::Light
	{
	public:
		struct RenderData
		{
			alignas(16) glm::vec3	Position;
			alignas(16) glm::vec3   DiffuseColor;
			alignas(16) glm::vec3   SpecularColor;
			float					Intensity;
			float					Distance;
			float					Falloff;
		};
	public:
		PointLight();
		virtual ~PointLight();
		static std::uint32_t GetTotalCount();

		void SetDistance(const float& distance);
		void SetFalloff(const float& falloff);

		const float& GetDistance()  const;
		float& GetDistance();
		const float& GetFalloff() const;
		float& GetFalloff();

		RenderData GetRenderData(const glm::vec3& position);
	protected:
		float		m_Distance;
		float		m_Falloff;
	private:
		static std::uint32_t  m_TotalCount;
	};
}


