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

/* OBB */
bool shade::PointLight::IsInside(const glm::vec3& position, const float& radius, const glm::mat4& transform, const glm::vec3& minHalfExt, const glm::vec3& maxHalfExt)
{
#if 1
	glm::vec3 t, r, scale;
	math::DecomposeMatrix(transform, t, r, scale);
	glm::vec3 localPos = glm::inverse(glm::scale(transform, glm::vec3(1.f))) * glm::vec4(position, 1.f);
#if 0
	float x = std::max(minHalfExt.x, std::min(realCenter.x, maxHalfExt.x));
	float y = std::max(minHalfExt.y, std::min(realCenter.y, maxHalfExt.y));
	float z = std::max(minHalfExt.z, std::min(realCenter.z, maxHalfExt.z));
	glm::vec3 closetPoint(x, y, z);
#else
	glm::vec3 closetPoint = glm::clamp(localPos, minHalfExt, maxHalfExt);
#endif
	glm::vec3 localPoint = closetPoint - localPos;

	return (glm::length(localPoint * scale) <= radius);
#else
	/* Without scale !*/
	/* Transform light position to obb local sapce without scale */
	glm::vec3 localPos = glm::inverse(glm::scale(transform, glm::vec3(1.f))) * glm::vec4(position, 1.f);
	/* Getting closet point */
	#if 1
	float x = std::max(minHalfExt.x, std::min(localPos.x, maxHalfExt.x));
	float y = std::max(minHalfExt.y, std::min(localPos.y, maxHalfExt.y));
	float z = std::max(minHalfExt.z, std::min(localPos.z, maxHalfExt.z));
	glm::vec3 closetPoint(x, y, z);
	#else
	glm::vec3 closetPoint = glm::clamp(localPos, minHalfExt, maxHalfExt);
	#endif
	glm::vec3 distance = closetPoint - localPos;
	return (glm::length(distance) <= radius)
#endif
}

shade::PointLight::RenderData shade::PointLight::GetRenderData(const glm::vec3& position)
{
	return RenderData{ position, m_DiffuseColor, m_SpecularColor, m_Intensity, m_Distance, m_Falloff};
}

std::uint32_t shade::PointLight::GetTotalCount()
{
	return m_TotalCount;
}
