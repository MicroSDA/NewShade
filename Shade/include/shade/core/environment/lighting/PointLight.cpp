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

int  shade::PointLight::m_Id      = 0;
int  shade::PointLight::m_Count   = 0;


shade::PointLight::PointLight() : shade::Light(shade::Environment::Type::PointLight),
	m_Position(0.0f, 0.0f, 0.0f),
	m_Constant(1.0f),
	m_Linear(0.7f),
	m_Qaudratic(1.8f)
{
	m_Count++;
}

shade::PointLight::~PointLight()
{
	m_Count--;
}

 void shade::PointLight::SetPosition(const float& x, const float& y, const float& z)
{
	m_Position = glm::vec3(x, y, z);
}

 void shade::PointLight::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

 const glm::vec3& shade::PointLight::GetPosition() const
{
	return m_Position;
}

glm::vec3& shade::PointLight::GetPosition()
{
	return const_cast<glm::vec3&>(const_cast<const shade::PointLight*>(this)->GetPosition());
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

void shade::PointLight::Process(const Shared<Shader>& shader)
{
	std::string id = std::to_string(m_Id);

	shader->SendInt("u_Lighting._PointLightsCount", m_Count);
	shader->SendFlaot3("u_Lighting._PointLights[" + id + "].Light.AmbientColor",     glm::value_ptr(m_AmbientColor));
	shader->SendFlaot3("u_Lighting._PointLights[" + id + "].Light.DiffuseColor",		glm::value_ptr(m_DiffuseColor));
	shader->SendFlaot3("u_Lighting._PointLights[" + id + "].Light.SpecularColor",	glm::value_ptr(m_SpecularColor));
	shader->SendFlaot3("u_Lighting._PointLights[" + id + "].Position",				glm::value_ptr(m_Position));
	shader->SendFlaot("u_Lighting._PointLights[" + id + "].Constant",				m_Constant);
	shader->SendFlaot("u_Lighting._PointLights[" + id + "].Linear",					m_Linear);
	shader->SendFlaot("u_Lighting._PointLights[" + id + "].Qaudratic",				m_Qaudratic);
	if (m_Id == m_Count - 1)
		m_Id = 0;
	else
		m_Id++;
}

void shade::PointLight::ResetId()
{
	m_Id = 0;
}
