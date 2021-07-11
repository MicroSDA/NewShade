#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API SpotLight :public shade::Light
	{
	public:
		 SpotLight();
		 virtual ~SpotLight();
		 void SetPosition(const float& x, const float& y, const float& z);
		 void SetPosition(const   glm::vec3& position);
		 void SetDirection(const float& x, const float& y, const float& z);
		 void SetDirection(const glm::vec3& direction);
		 void SetMinAngle(const float& angle);
		 void SetMaxAngle(const float& angle);
		 void SetConstant(const float& constant);
		 void SetLinear(const float& linear);
		 void SetQaudratic(const float& qaudratic);

		 const glm::vec3& GetPosition() const;
		 glm::vec3& GetPosition();
		
		 const float& GetMinAngle() const;
		 float& GetMinAngle();
		 const float& GetMaxAngle() const;
		 float& GetMaxAngle();
		 const glm::fvec3& GetDirection() const;
		 glm::fvec3& GetDirection();

		 const float& GetConstant() const;
		 float& GetConstant();
		 const float& GetLinear() const;
		 float& GetLinear();
		 const float& GetQaudratic() const;
		 float& GetQaudratic();

		virtual void Process(const Shared<Shader>& shader) override;
	private:
		glm::fvec3	m_Direction;
		glm::fvec3	m_Position;
		float		m_Constant;
		float		m_Linear;
		float		m_Qaudratic;
		float		m_MinAngle;
		float		m_MaxAngle;
		static int  m_Id;
		static int  m_Count;
	};

}

