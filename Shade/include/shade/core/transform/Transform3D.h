#pragma once
#define GLM_ENABLE_EXPEREMENTAL // for quaternion.hpp
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "shade/core/math/Math.h"

namespace shade
{

	class SHADE_API Transform3D
	{
	public:
		Transform3D();
		Transform3D(const glm::fvec3& position, const glm::fvec3& rotation, const glm::fvec3& scale);
		~Transform3D() = default;
		inline void SetPostition(const float& x, const float& y, const float& z) { m_Possition.x = x; m_Possition.y = y; m_Possition.z = z; };
		inline void SetPostition(const glm::fvec3& position)                      { m_Possition = position;};
		inline void SetRotation(const float& x, const float& y, const float& z)  { m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z;};
		inline void SetRotation(const glm::fvec3& rotation)                       { m_Rotation = rotation; };
		inline void SetScale(const float& x, const float& y, const float& z)     { m_Scale.x = x; m_Scale.y = y; m_Scale.z = z;};
		inline void SetScale(const glm::fvec3& scale)                             { m_Scale = scale; };

		inline const glm::fvec3& GetPosition() const { return m_Possition; };
		inline const glm::fvec3& GetRotation() const { return m_Rotation; };
		inline const glm::fvec3& GetScale()    const { return m_Scale; };

		inline glm::fvec3& GetPosition() { return const_cast<glm::fvec3&>(const_cast<const shade::Transform3D*>(this)->GetPosition()); };
		inline glm::fvec3& GetRotation() { return const_cast<glm::fvec3&>(const_cast<const shade::Transform3D*>(this)->GetRotation()); };
		inline glm::fvec3& GetScale()    { return const_cast<glm::fvec3&>(const_cast<const shade::Transform3D*>(this)->GetScale());    };

		glm::mat4 GetModelMatrix() const;
		void	  SetTransform(const glm::mat4& matrix)
		{
			math::DecomposeMatrix(matrix, m_Possition, m_Rotation, m_Scale);
		}
		static Transform3D GetTransformFromMatrix(const glm::mat4& matrix)
		{
			Transform3D transform;
			math::DecomposeMatrix(matrix, transform.m_Possition, transform.m_Rotation, transform.m_Scale);
			return transform;
		}
	private:
		glm::fvec3 m_Possition;
		glm::fvec3 m_Rotation;
		glm::fvec3 m_Scale;
		
	};
}

