#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API SpotLight :public shade::Light
	{
	public:
		struct RenderData
		{
			alignas(16) glm::vec3	Position;
			alignas(16) glm::vec3	Direction;
			alignas(16) glm::vec3   DiffuseColor;
			alignas(16) glm::vec3   SpecularColor;
			float					Intesity;
			float					Distance;
			float					Falloff;
			float					MinAngle;
			float					MaxAngle;
		};
	public:
		 SpotLight();
		 virtual ~SpotLight();
		 static std::uint32_t GetTotalCount();

		 void SetMinAngle(const float& angle);
		 void SetMaxAngle(const float& angle);
		 void SetDistance(const float& distance);
		 void SetFalloff(const float& falloff);

		
		 const float& GetMinAngle() const;
		 float& GetMinAngle();
		 const float& GetMaxAngle() const;
		 float& GetMaxAngle();

		 const float& GetDistance() const;
		 float& GetDistance();
		 const float& GetFalloff() const;
		 float& GetFalloff();

		 RenderData GetRenderData(const glm::vec3& position, const glm::vec3& derection) const;
	private:
		float		m_Distance;
		float		m_Falloff;
		float		m_MinAngle;
		float		m_MaxAngle;
		static std::uint32_t  m_TotalCount;	
	};
}

