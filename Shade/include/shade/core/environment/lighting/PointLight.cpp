#include "shade_pch.h"
#include "PointLight.h"
/*  Range Constant Linear  Quadratic

	3250, 1.0,     0.0014  0.000007

	600,  1.0,     0.007   0.0002

	325,  1.0,     0.014,  0.0007

	200,  1.0,     0.022,  0.0019

	160,  1.0,     0.027,  0.0028

	100,  1.0,     0.045,  0.0075

	65,   1.0,     0.07,   0.017

	50,   1.0,     0.09,   0.032

	32,   1.0,     0.14,   0.07

	20,   1.0,     0.22,   0.20

	13,   1.0,     0.35,   0.44

	7,    1.0,     0.7,    1.8
*/

std::uint32_t  shade::PointLight::m_TotalCount = 0;


shade::PointLight::PointLight() : shade::Light(shade::Environment::Type::PointLight),
	m_Constant(1.0f),
	m_Linear(0.7f),
	m_Qaudratic(1.8f)
{
	// Icnrease count
	m_TotalCount++;
}

shade::PointLight::~PointLight()
{
	// Decrease light count
	m_TotalCount--;
}

 void shade::PointLight::SetConstant(const float& constant)
{
	m_Constant = constant;
}

 void shade::PointLight::SetLinear(const float& linear)
{
	m_Linear = linear;
}

 void shade::PointLight::SetQaudratic(const float& qaudratic)
{
	m_Qaudratic = qaudratic;
}

 const float& shade::PointLight::GetConstant() const
{
	return m_Constant;
}

float& shade::PointLight::GetConstant()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetConstant());
}

 const float& shade::PointLight::GetLinear() const
{
	return  m_Linear;
}

float& shade::PointLight::GetLinear()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetLinear());
}

 const float& shade::PointLight::GetQaudratic() const
{
	return m_Qaudratic;
}

float& shade::PointLight::GetQaudratic()
{
	return const_cast<float&>(const_cast<const shade::PointLight*>(this)->GetQaudratic());
}

shade::PointLight::RenderData shade::PointLight::GetRenderData(const glm::vec3& position)
{
	return RenderData{ position, m_AmbientColor, m_DiffuseColor, m_SpecularColor, m_Constant, m_Linear, m_Qaudratic};
}

std::uint32_t shade::PointLight::GetTotalCount()
{
	return m_TotalCount;
}
