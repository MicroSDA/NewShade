#include "shade_pch.h"
#include "PointLight.h"
#include "shade/core/math/Math.h"
#include <glm/gtx/rotate_vector.hpp >

std::uint32_t  shade::PointLight::m_TotalCount = 0;

shade::PointLight::PointLight() : shade::Light(shade::Environment::Type::PointLight),
	m_Distance(10.0f),
	m_Falloff(0.25f)
{
	// Icnrease count
	m_TotalCount++;
}

shade::PointLight::~PointLight()
{
	// Decrease light count
	m_TotalCount--;
}

 void shade::PointLight::SetDistance(const float& distnace)
{
	m_Distance = distnace;
}

 void shade::PointLight::SetFalloff(const float& falloff)
{
	m_Falloff = falloff;
}

 const float& shade::PointLight::GetDistance() const
{
	return  m_Distance;
}

float& shade::PointLight::GetDistance()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetDistance());
}

 const float& shade::PointLight::GetFalloff() const
{
	return m_Falloff;
}

float& shade::PointLight::GetFalloff()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetFalloff());
}

bool shade::PointLight::IsInside(const glm::vec3& position, const float& radius, const glm::mat4& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
	
	

	/*glm::vec4 min, max;
	min = transform * glm::vec4(minHalfExt, 1.f);
	max = transform * glm::vec4(maxHalfExt, 1.f);

	if (position.x - min.x >= radius
		&& position.y - min.y >= radius
		&& position.z - min.z >= radius
		&& max.x - position.x >= radius
		&& max.y - position.y >= radius
		&& max.z - position.z >= radius)
	{
		return true;
	}
	double dmin = 0;
	double e = position.x - min.x;
	if (e < 0)
	{
		if (e < -radius)
		{
			return false;
		}
		dmin += e * e;
	}
	else
	{
		e = position.x - max.x;
		if (e > 0)
		{
			if (e > radius)
			{
				return false;
			}
			dmin += e * e;
		}
	}
	e = position.y - min.y;
	if (e < 0)
	{
		if (e < -radius)
		{
			return false;
		}
		dmin += e * e;
	}
	else
	{
		e = position.y - max.y;
		if (e > 0)
		{
			if (e > radius)
			{
				return false;
			}
			dmin += e * e;
		}
	}
	e = position.z - min.z;
	if (e < 0)
	{
		if (e < -radius)
		{
			return false;
		}
		dmin += e * e;
	}
	else
	{
		e = position.z - max.z;
		if (e > 0)
		{
			if (e > radius)
			{
				return false;
			}
			dmin += e * e;
		}
	}
	return dmin <= radius * radius;*/





	//glm::mat4 matrix = (transform);
	//glm::vec4 min, max;
	//min = (matrix) * glm::vec4(minHalfExt, 1.f);
	//max = (matrix) * glm::vec4(maxHalfExt, 1.f);

	///* If transposed*/
	////glm::vec3 closestPoint = glm::vec3(matrix[0][3], matrix[1][3], matrix[2][3]); // Start at the center point of the OBB.
	////glm::vec3 d = position - glm::vec3(matrix[0][3], matrix[1][3], matrix[2][3]);

	//glm::vec3 closestPoint = glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]); // Start at the center point of the OBB.
	//glm::vec3 d = position - glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);

	//glm::vec3 axis[3] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) };
	//for (int i = 0; i < 3; ++i) // Project the target onto the OBB axes and walk towards that point.
	//	closestPoint += glm::clamp(glm::dot(d, axis[i]), min[i], max[i]) * axis[i];

	////closestPoint = (matrix) * glm::vec4(closestPoint, 1.f);
	//return glm::distance(closestPoint, position) <= radius * radius;
	

	/*glm::vec3 axies[3] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0) ,glm::vec3(0, 0, 1) };
	glm::vec3 minExt, maxExt, pos;
	minExt	= glm::transpose(transform) * glm::vec4(minHalfExt, 1.f);
	maxExt	= glm::transpose(transform) * glm::vec4(maxHalfExt, 1.f);

	glm::vec3 retPt;
	glm::vec3 d = position - glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	retPt = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	for (int i = 0; i < 3; ++i)
	{
		float dist = glm::dot(d, axies[i]);
		if (dist > maxExt[i])  dist = maxExt[i];
		if (dist < minExt[i])  dist = minExt[i];
		retPt += dist * axies[i];
	}

	glm::vec3 v = retPt - position;
	return glm::distance(v, position) <= radius;*/


	//glm::vec3 minExt, maxExt, pos;
	//glm::mat4 matrix = glm::transpose(transform);
	//minExt	= matrix * glm::vec4(minHalfExt, 1.f);
	//maxExt	= matrix * glm::vec4(maxHalfExt, 1.f);

	//pos	    = transform * glm::vec4(position, 1.f);

	//float x = std::max(minExt.x, std::min(pos.x, maxExt.x));
	//float y = std::max(minExt.y, std::min(pos.y, maxExt.y));
	//float z = std::max(minExt.z, std::min(pos.z, maxExt.z));

	//// this is the same as isPointInsideSphere
	//float distance = glm::sqrt((x - pos.x) * (x - pos.x) +
	//	(y - pos.y) * (y - pos.y) +
	//	(z - pos.z) * (z - pos.z));

	//return distance <= radius;

#if 1
	glm::vec3 t, r, s;
	math::DecomposeMatrix(transform, t, r, s);

	glm::vec3 center		= position;
	glm::vec3 realCenter	= glm::inverse(glm::scale(transform, glm::vec3(1.f))) * glm::vec4(center, 1.f);
	SHADE_CORE_DEBUG("Center x {0} y {1} z {2}", realCenter.x, realCenter.y, realCenter.z);
	glm::vec3 min			=  minHalfExt;
	glm::vec3 max			=  maxHalfExt;

	glm::vec3				halfSclaed = glm::vec3(max - min) / 2.f;

	float x = std::max(min.x, std::min(realCenter.x, max.x));
	float y = std::max(min.y, std::min(realCenter.y, max.y));
	float z = std::max(min.z, std::min(realCenter.z, max.z));

	glm::vec3 closetPoint(x, y, z);

	glm::vec3 localPoint = closetPoint - realCenter;

	if (glm::length(localPoint * s) <= radius)
		return true;
	else
		return false;

#else
	glm::mat4 matrix = (transform);
	glm::vec3 min = matrix * glm::vec4(minHalfExt, 1.f), max = matrix * glm::vec4(maxHalfExt, 1.f);
	//glm::vec3 delta = position - ( glm::vec3(min + max) / 2.f );
	glm::vec3 delta = glm::vec4(position,1.f) * glm::inverse(transform);
	glm::vec3 boxSize = glm::vec3(max - min) / 2.f;
	glm::vec3 closetPointOnBox = glm::clamp(delta, -boxSize, boxSize);
	glm::vec3 localPoint = delta - closetPointOnBox;
	if (glm::length(localPoint) <= radius)
	return true;
		else
	return false;
#endif
	
	//float sqDist = 0.0f;
	//for (int i = 0; i < 3; i++) {
	//	// for each axis count any excess distance outside box extents
	//	float v = pos[i];
	//	if (v < min[i]) 
	//		sqDist += (v - min[i]) * (v - min[i]);
	//	else if (v > max[i])
	//		sqDist += (v + max[i]) * (v + max[i]);
	//}

	//return sqDist <= radius * radius;

	//glm::mat4 clipSpaceMatrix = glm::transpose(transform);
	//glm::vec4 points[8];
	//points[0] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);
	//points[1] = clipSpaceMatrix * glm::vec4(minHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);
	//points[2] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, maxHalfExt.z, 1.f);
	//points[3] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, maxHalfExt.y, minHalfExt.z, 1.f);
	//points[4] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);
	//points[5] = clipSpaceMatrix * glm::vec4(maxHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);
	//points[6] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, maxHalfExt.z, 1.f);
	//points[7] = clipSpaceMatrix * glm::vec4(minHalfExt.x, minHalfExt.y, minHalfExt.z, 1.f);

	//bool result = true;
	//// тестируем 6 плоскостей фрустума
	//for (int i = 0; i < 8; i++)
	//{
	//	if (points[i].x * position.x + points[i].y * position.y + points[i].z * position.z + points[i].w <= -radius)
	//		result = false;
	//}
	//return result;
}

shade::PointLight::RenderData shade::PointLight::GetRenderData(const glm::vec3& position)
{
	return RenderData{ position, m_DiffuseColor, m_SpecularColor, m_Intensity, m_Distance, m_Falloff};
}

std::uint32_t shade::PointLight::GetTotalCount()
{
	return m_TotalCount;
}
