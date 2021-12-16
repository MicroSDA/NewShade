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
			alignas(16) glm::vec3   AmbientColor;
			alignas(16) glm::vec3   DiffuseColor;
			alignas(16) glm::vec3   SpecularColor;
			float					Constant;
			float					Linear;
			float					Qaudratic;
		};
	public:
		PointLight();
		virtual ~PointLight();
		static std::uint32_t GetTotalCount();

		void SetConstant(const float& constant);
		void SetLinear(const float& linear);
		void SetQaudratic(const float& qaudratic);

		const float& GetConstant() const;
		float& GetConstant();
		const float& GetLinear()  const;
		float& GetLinear();
		const float& GetQaudratic() const;
		float& GetQaudratic();

		RenderData GetRenderData(const glm::vec3& position);
	protected:
		float		m_Constant;
		float		m_Linear;
		float		m_Qaudratic;
	private:
		static std::uint32_t  m_TotalCount;
	};
}


