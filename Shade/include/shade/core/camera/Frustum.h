#pragma once
#include "shade/config/API.h"
#include "shade/core/transform/Transform3D.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		~Frustum() = default;
		const glm::vec4&  GetSide(const Frustum::Side& size) const;
		const glm::vec4*  GetFrustum() const;

		//For AABB 
		bool IsInFrustum(const Transform3D& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
	private:
		void _CalculateFrustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
		void _Normalize(glm::vec4& side);
		bool _AABBTest(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		bool _OBBTest(const Transform3D& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt);
		//bool _SSE_AABBTest(const glm::vec4& minExt, const glm::vec4& maxExt);
	private:
		glm::mat4 m_VP_Matrix; // Camera View and Projection product
		glm::vec4 m_Frustum[6];
	};
}


