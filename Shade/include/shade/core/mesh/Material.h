#pragma once
#include "shade/config/API.h"
#include "shade/core/render/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	class SHADE_API Material
	{
	public:
		Material();
		virtual ~Material();

		void SetName(const std::string& name);

		void SetAmbientColor(const float& r, const float& g, const float& b);
		void SetAmbientColor(const glm::vec3& color);

		void SetDiffuseColor(const float& r, const float& g, const float& b);
		void SetDiffuseColor(const glm::vec3& color);

		void SetSpecularColor(const float& r, const float& g, const float& b);
		void SetSpecularColor(const glm::vec3& color);

		void SetTransparentMask(const float& r, const float& g, const float& b);
		void SetTransparentMask(const glm::vec3& color);

		void SetShininess(const float& value);
		void SetShininessStrength(const float& value);

		const std::string& GetName() const;

		const glm::vec3& GetAmbientColor() const;
		glm::vec3& GetAmbientColor();

		const glm::vec3& GetDiffuseColor() const;
		glm::vec3& GetDiffuseColor();

		const glm::vec3& GetSpecularColor() const;
		glm::vec3& GetSpecularColor();

		const glm::vec3& GetTransparentMask() const;
		glm::vec3& GetTransparentMask();

		const float& GetShininess() const;
		float& GetShininess();

		const float& GetShininessStrength() const;
		float& GetShininessStrength();

		void Process(const Shared<Shader>& shader) const;
	private:
		std::string m_Name;
		glm::vec3   m_AmbientColor;
		glm::vec3   m_DiffuseColor;
		glm::vec3   m_SpecularColor;
		glm::vec3   m_TransparentMask;
		float       m_ShininessStrength;
		float       m_Shininess;
	};
}