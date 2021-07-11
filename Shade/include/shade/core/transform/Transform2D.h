#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace shade
{
	class SHADE_API Transform2D
	{
	public:
		Transform2D();
		~Transform2D() = default;

		inline void SetPostition(const float& x, const float& y) { m_Possition.x = x; m_Possition.y = y; };
		inline void SetPostition(const glm::vec2& position)		 { m_Possition = position; };
		inline void SetRotation(const float& x, const float& y)  { m_Rotation.x = x; m_Rotation.y = y; };
		inline void SetRotation(const glm::vec2& rotation)	     { m_Rotation = rotation; };
		inline void SetScale(const float& x, const float& y)     { m_Scale.x = x; m_Scale.y = y; };
		inline void SetScale(const glm::vec2& scale)			 { m_Scale = scale; };

		inline const glm::vec2& GetPosition() const { return m_Possition; };
		inline const glm::vec2& GetRotation() const { return m_Rotation; };
		inline const glm::vec2& GetScale()    const { return m_Scale; };

		const glm::mat4 GetModelMatrix()      const;
	private:
		glm::vec2 m_Possition;
		glm::vec2 m_Rotation;
		glm::vec2 m_Scale;
	};

}

