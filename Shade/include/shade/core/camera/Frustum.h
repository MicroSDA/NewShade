#pragma once
#include "shade/config/API.h"
#include "shade/core/transform/Transform3D.h"
#include "shade/core/math/Math.h"

namespace shade
{
	class SHADE_API Frustum
	{
	public:
		enum class Side : std::uint32_t
		{
			Left, Right, Top, Bottom, Near, Far
		};
	public:
		Frustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
		Frustum(const glm::mat4& viewProjectionMatrix);
		~Frustum() = default;
		const glm::vec4&        GetSide(const Frustum::Side& side) const;
		const std::vector<glm::vec4>& GetFrustum() const;
		//For AABB 
		bool IsInFrustum(const glm::mat4& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool IsInFrustum(const glm::vec3& position,  const float& radius);
	private:
		void _CalculateFrustum(const glm::mat4& viewProjectionMatrix);
		void _Normalize(glm::vec4& side);
		bool _AABBTest(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool _SSE_AABBTest(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool _OBBTest(const glm::mat4& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool _SSE_OBBTest(const glm::mat4& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool _SPHERE_TEST(const glm::vec3& position, const float& radius);
	private:
		glm::mat4 m_VP_Matrix; // Camera View and Projection product
		std::vector<glm::vec4> m_Frustum;
		//glm::vec4 m_Frustum[6];
	};
}


