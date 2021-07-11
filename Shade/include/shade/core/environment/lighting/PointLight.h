#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API PointLight : public shade::Light
	{
	public:
		PointLight();
		virtual ~PointLight();
		void SetPosition(const float& x, const float& y, const float& z);
		void SetPosition(const   glm::vec3& position);
		void SetConstant(const float& constant);
		void SetLinear(const float& linear);
		void SetQaudratic(const float& qaudratic);

		const glm::vec3& GetPosition() const;
		glm::vec3& GetPosition();
		const float& GetConstant() const;
		float& GetConstant();
		const float& GetLinear()  const;
		float& GetLinear();
		const float& GetQaudratic() const;
		float& GetQaudratic();

		virtual void Process(const Shared<Shader>& shader) override;
		static void  ResetId();
	protected:
		glm::vec3	m_Position;
		float		m_Constant;
		float		m_Linear;
		float		m_Qaudratic;
		static int  m_Id;
		static int  m_Count;
	};
}


