#pragma once
#include "shade/core/environment/Environment.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	class SHADE_API Light : public shade::Environment
	{
	public:
		Light(const shade::Environment::Type& type);
		virtual ~Light() = default;
		void SetAmbientColor(const  float& r, const float& g, const float& b);
		void SetDiffuseColor(const  float& r, const float& g, const float& b);
		void SetSpecularColor(const float& r, const float& g, const float& b);
		void SetAmbientColor(const   glm::vec3& color);
		void SetDiffuseColor(const   glm::vec3& color);
		void SetSpecularColor(const  glm::vec3& color);
		const glm::vec3& GetAmbientColor() const;
		const glm::vec3& GetDiffuseColor() const;
		const glm::vec3& GetSpecularColor() const;

		glm::vec3& GetAmbientColor();
		glm::vec3& GetDiffuseColor();
		glm::vec3& GetSpecularColor();
	protected:
		glm::vec3  m_AmbientColor;
		glm::vec3  m_DiffuseColor;
		glm::vec3  m_SpecularColor;
	private:
	};

}