#pragma once
#define GLM_ENABLE_EXPEREMENTAL // for quaternion.hpp
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace shade
{

	class SHADE_API Transform3D
	{
	public:
		Transform3D();
		~Transform3D() = default;
		inline void SetPostition(const float& x, const float& y, const float& z) { m_Possition.x = x; m_Possition.y = y; m_Possition.z = z; };
		inline void SetPostition(const glm::vec3& position)                      { m_Possition = position;};
		inline void SetRotation(const float& x, const float& y, const float& z)  { m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z;};
		inline void SetRotation(const glm::vec3& rotation)                       { m_Rotation = rotation; };
		inline void SetScale(const float& x, const float& y, const float& z)     { m_Scale.x = x; m_Scale.y = y; m_Scale.z = z;};
		inline void SetScale(const glm::vec3& scale)                             { m_Scale = scale; };

		inline const glm::vec3& GetPosition() const { return m_Possition; };
		inline const glm::vec3& GetRotation() const { return m_Rotation; };
		inline const glm::vec3& GetScale()    const { return m_Scale; };

		inline glm::vec3& GetPosition() { return const_cast<glm::vec3&>(const_cast<const shade::Transform3D*>(this)->GetPosition()); };
		inline glm::vec3& GetRotation() { return const_cast<glm::vec3&>(const_cast<const shade::Transform3D*>(this)->GetRotation()); };
		inline glm::vec3& GetScale()    { return const_cast<glm::vec3&>(const_cast<const shade::Transform3D*>(this)->GetScale()); };

		glm::mat4 GetModelMatrix() const;
	
	private:
		glm::vec3 m_Possition;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;
		
	};
}

