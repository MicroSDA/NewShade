#pragma once
#include "shade/core/environment/lighting/Light.h"

namespace shade
{
	class SHADE_API DirectLight : public shade::Light
	{
	public:
		DirectLight();
		virtual ~DirectLight() = default;
		void SetDirection(const float& x, const float& y, const float& z);
		void SetDirection(const glm::fvec3& direction);
		const glm::fvec3& GetDirection() const;
		glm::fvec3& GetDirection();
	
		virtual void Process(const Shared<Shader>& shader) override;
	protected:
		glm::fvec3 m_Direction;
	private:	

	
	};

}


