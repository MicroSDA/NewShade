#include "shade_pch.h"
#include "Frustum.h"

shade::Frustum::Frustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
	_CalculateFrustum(viewMatrix, projMatrix);
}
const glm::vec4* shade::Frustum::GetFrustum() const
{
	return &m_Frustum[0];
}

const glm::vec4& shade::Frustum::GetSide(const Frustum::Side& size) const
{
	return m_Frustum[(std::uint32_t)size];
}

bool shade::Frustum::IsInFrustum(const Transform3D& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	// Set min and max ext in world possition by transform GetPosition()
	//return _AABBTest(glm::vec3(transform.GetPosition() - halfExt), glm::vec3(transform.GetPosition() + halfExt)); // In this case that we using position and + half extension need to be tested
	// With rotation and scale for dynamic object, slower than AABB
	//return _OBBTest(transform, minHalfExt, maxHalfExt);
	// SSE optimization
	return _SSE_OBBTest(transform, minHalfExt, maxHalfExt);
}

void shade::Frustum::_CalculateFrustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
	m_VP_Matrix = projMatrix * viewMatrix;
	glm::mat4 matrix = glm::transpose(m_VP_Matrix);

	m_Frustum[(std::uint32_t)Side::Left]   = matrix[3] + matrix[0];
	m_Frustum[(std::uint32_t)Side::Right]  = matrix[3] - matrix[0];
	m_Frustum[(std::uint32_t)Side::Bottom] = matrix[3] + matrix[1];
	m_Frustum[(std::uint32_t)Side::Top]    = matrix[3] - matrix[1];
	m_Frustum[(std::uint32_t)Side::Near]   = matrix[3] + matrix[2];
	m_Frustum[(std::uint32_t)Side::Far]    = matrix[3] - matrix[2];

	// Normalizing x,y,z
	for (auto i = 0; i < 6; i++)
	{
		_Normalize(m_Frustum[i]);
	}
}

void shade::Frustum::_Normalize(glm::vec4& side)
{	/* Only for x, y, z*/
	float magnitude = (float)sqrt(side.x * side.x + side.y * side.y + side.z * side.z);
	side.x /= magnitude;
	side.y /= magnitude;
	side.z /= magnitude;
	side.w /= magnitude;
}

bool shade::Frustum::_AABBTest(const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	bool inside = true;
	//тестируем 6 плоскостей фрустума
	for (int i = 0; i < 6; i++)
	{
		//находим ближайшую к плоскости вершину
		//провер€ем, если она находитс€ за плоскостью, то объект вне врустума
		float d = std::max(minHalfExt.x * m_Frustum[i].x, maxHalfExt.x * m_Frustum[i].x)
				+ std::max(minHalfExt.y * m_Frustum[i].y, maxHalfExt.y * m_Frustum[i].y)
				+ std::max(minHalfExt.z * m_Frustum[i].z, maxHalfExt.z * m_Frustum[i].z)
				+ m_Frustum[i].w;
		inside &= d > 0;
	}
	//если не нашли раздел€ющей плоскости, считаем объект видим
	return inside;
}

bool shade::Frustum::_OBBTest(const Transform3D& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	//transform all 8 box points to clip space
	//clip space because we easily can test points outside required unit cube
	//NOTE: for DirectX we should test z coordinate from 0 to w (-w..w - for OpenGL), look for transformations / clipping box differences

	//matrix to transfrom points to clip space
	glm::mat4 clipSpaceMatrix = m_VP_Matrix * transform.GetModelMatrix();

	//transform all 8 box points to clip space
	glm::vec4 points[8];
	points[0] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);
	points[1] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);
	points[2] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);
	points[3] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);
	points[4] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);
	points[5] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);
	points[6] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);
	points[7] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);

	bool outside = false, outsidePositivePlane, outsideNegativePlane;
	//we have 6 frustum planes, which in clip space is unit cube (for GL) with -1..1 range
	for (int i = 0; i < 3; i++) //3 because we test positive & negative plane at once
	{
		//if all 8 points outside one of the plane
		//actually it is vertex normalization xyz / w, then compare if all 8points coordinates < -1 or > 1
		outsidePositivePlane =
			points[0][i] > points[0].w &&
			points[1][i] > points[1].w &&
			points[2][i] > points[2].w &&
			points[3][i] > points[3].w &&
			points[4][i] > points[4].w &&
			points[5][i] > points[5].w &&
			points[6][i] > points[6].w &&
			points[7][i] > points[7].w;

		outsideNegativePlane =
			points[0][i] < -points[0].w &&
			points[1][i] < -points[1].w &&
			points[2][i] < -points[2].w &&
			points[3][i] < -points[3].w &&
			points[4][i] < -points[4].w &&
			points[5][i] < -points[5].w &&
			points[6][i] < -points[6].w &&
			points[7][i] < -points[7].w;

		outside = outside || outsidePositivePlane || outsideNegativePlane;
	}
	return !outside;
}

bool shade::Frustum::_SSE_OBBTest(const Transform3D& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	math::sse_mat4f clipSpaceMatrix = math::sse_mat4f(m_VP_Matrix) * math::sse_mat4f(transform.GetModelMatrix());
	//box points in local space
	math::sse_vec4f obb_points_sse[8];
	obb_points_sse[0] = _mm_set_ps(1.f, minHalfExt.z, maxHalfExt.y, minHalfExt.x);
	obb_points_sse[1] = _mm_set_ps(1.f, maxHalfExt.z, maxHalfExt.y, minHalfExt.x);
	obb_points_sse[2] = _mm_set_ps(1.f, maxHalfExt.z, maxHalfExt.y, maxHalfExt.x);
	obb_points_sse[3] = _mm_set_ps(1.f, minHalfExt.z, maxHalfExt.y, maxHalfExt.x);
	obb_points_sse[4] = _mm_set_ps(1.f, minHalfExt.z, minHalfExt.y, maxHalfExt.x);
	obb_points_sse[5] = _mm_set_ps(1.f, maxHalfExt.z, minHalfExt.y, maxHalfExt.x);
	obb_points_sse[6] = _mm_set_ps(1.f, maxHalfExt.z, minHalfExt.y, minHalfExt.x);
	obb_points_sse[7] = _mm_set_ps(1.f, minHalfExt.z, minHalfExt.y, minHalfExt.x);

	__m128 zero_v = _mm_setzero_ps();
	//initially assume that planes are separating
	//if any axis is separating - we get 0 in certain outside_* place
	__m128 outside_positive_plane = _mm_set1_ps(-1.f); //NOTE: there should be negative value..
	__m128 outside_negative_plane = _mm_set1_ps(-1.f); //because _mm_movemask_ps (while storing result) cares abount 'most significant bits' (it is sign of float value)

	//for all 8 box points
	for (auto i = 0u; i < 8; i++)
	{
		//transform point to clip space
		math::sse_vec4f obb_transformed_point = clipSpaceMatrix * obb_points_sse[i];

		//gather w & -w
		__m128 wwww = _mm_shuffle_ps(obb_transformed_point.xyzw, obb_transformed_point.xyzw, _MM_SHUFFLE(3, 3, 3, 3)); //get w
		__m128 wwww_neg = _mm_sub_ps(zero_v, wwww);  // negate all elements

		//box_point.xyz > box_point.w || box_point.xyz < -box_point.w ?
		//similar to point normalization: point.xyz /= point.w; And compare: point.xyz > 1 && point.xyz < -1
		__m128 outside_pos_plane = _mm_cmpge_ps(obb_transformed_point.xyzw, wwww);
		__m128 outside_neg_plane = _mm_cmple_ps(obb_transformed_point.xyzw, wwww_neg);

		//if at least 1 of 8 points in front of the plane - we get 0 in outside_* flag
		outside_positive_plane = _mm_and_ps(outside_positive_plane, outside_pos_plane);
		outside_negative_plane = _mm_and_ps(outside_negative_plane, outside_neg_plane);
	}

	//all 8 points xyz < -1 or > 1 ?
	__m128 outside = _mm_or_ps(outside_positive_plane, outside_negative_plane);

	//store result, if any of 3 axes is separating (i.e. outside != 0) - object outside frustum
	//so, object inside frustum only if outside == 0 (there are no separating axes)
	return ((_mm_movemask_ps(outside) & 0x7) == 0); //& 0x7 mask, because we interested only in 3 axes
}
