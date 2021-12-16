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
			alignas(16) glm::vec3   AmbientColor;
			alignas(16) glm::vec3   DiffuseColor;
			alignas(16) glm::vec3   SpecularColor;
			float					Constant;
			float					Linear;
			float					Qaudratic;
			float					MinAngle;
			float					MaxAngle;
		};
	public:
		 SpotLight();
		 virtual ~SpotLight();
		 static std::uint32_t GetTotalCount();

		 void SetMinAngle(const float& angle);
		 void SetMaxAngle(const float& angle);
		 void SetConstant(const float& constant);
		 void SetLinear(const float& linear);
		 void SetQaudratic(const float& qaudratic);

		
		 const float& GetMinAngle() const;
		 float& GetMinAngle();
		 const float& GetMaxAngle() const;
		 float& GetMaxAngle();

		 const float& GetConstant() const;
		 float& GetConstant();
		 const float& GetLinear() const;
		 float& GetLinear();
		 const float& GetQaudratic() const;
		 float& GetQaudratic();

		 RenderData GetRenderData(const glm::vec3& position, const glm::vec3& derection) const;
	private:
		float		m_Constant;
		float		m_Linear;
		float		m_Qaudratic;
		float		m_MinAngle;
		float		m_MaxAngle;
		static std::uint32_t  m_TotalCount;	
	};
}

