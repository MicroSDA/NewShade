#include "shade_pch.h"
#include "SpotLight.h"
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
int  shade::SpotLight::m_Id = 0;
int  shade::SpotLight::m_Count = 0;
shade::SpotLight::SpotLight() : shade::Light(shade::Environment::Type::SpotLight),
	m_Direction(0.0, 0.0f, 1.0f),
	m_Position(0.0f, 0.0f, 0.0f),
	m_Constant(1.0f),
	m_Linear(0.007f),
	m_Qaudratic(0.00007f),
	m_MinAngle(glm::cos(glm::radians(5.5f))),
	m_MaxAngle(glm::cos(glm::radians(7.5f)))
{
	m_Count++;
}

shade::SpotLight::~SpotLight()
{
	m_Count--;
}

 void shade::SpotLight::SetPosition(const float& x, const float& y, const float& z)
{
	m_Position = glm::vec3(x, y, z);
}

 void shade::SpotLight::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

 const glm::vec3& shade::SpotLight::GetPosition() const
{
	return m_Position;
}

 glm::vec3& shade::SpotLight::GetPosition()
 {
	 return const_cast<glm::vec3&>(const_cast<const shade::SpotLight*>(this)->GetPosition());
 }

 void shade::SpotLight::SetDirection(const float& x, const float& y, const float& z)
{
	m_Direction = glm::vec3(x, y, z);
}

 void shade::SpotLight::SetDirection(const glm::vec3& direction)
{
	m_Direction = direction;
}

 void shade::SpotLight::SetMinAngle(const float& angle)
{
	m_MinAngle = angle;
}

 void shade::SpotLight::SetMaxAngle(const float& angle)
{
	m_MaxAngle = angle;
}

 const float& shade::SpotLight::GetMinAngle() const
{
	return m_MinAngle;
}

 float& shade::SpotLight::GetMinAngle()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetMinAngle());
 }

 const float& shade::SpotLight::GetMaxAngle() const
 {
	 return m_MaxAngle;
 }

 float& shade::SpotLight::GetMaxAngle()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetMaxAngle());
 }

 const glm::fvec3& shade::SpotLight::GetDirection() const
{
	return m_Direction;
}

 glm::fvec3& shade::SpotLight::GetDirection()
 {
	 return const_cast<glm::fvec3&>(const_cast<const shade::SpotLight*>(this)->GetDirection());
 }

 void shade::SpotLight::SetConstant(const float& constant)
 {
	 m_Constant = constant;
 }

 void shade::SpotLight::SetLinear(const float& linear)
 {
	 m_Linear = linear;
 }

 void shade::SpotLight::SetQaudratic(const float& qaudratic)
 {
	 m_Qaudratic = qaudratic;
 }

 const float& shade::SpotLight::GetConstant() const
 {
	 return m_Constant;
 }

 float& shade::SpotLight::GetConstant()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetConstant());
 }

 const float& shade::SpotLight::GetLinear() const
 {
	 return m_Linear;
 }

 float& shade::SpotLight::GetLinear()
 {
	 return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetLinear());
 }

const float& shade::SpotLight::GetQaudratic() const
{
	return m_Qaudratic;
}

float& shade::SpotLight::GetQaudratic()
{
	return const_cast<float&>(const_cast<const shade::SpotLight*>(this)->GetQaudratic());
}

void shade::SpotLight::Process(const Shared<Shader>& shader)
{
	std::string id = std::to_string(m_Id);

	shader->SendInt("u_Lighting._SpotLightsCount", m_Count);
	shader->SendFlaot3("u_Lighting._SpotLights[" + id + "].Light.Light.AmbientColor",	glm::value_ptr(m_AmbientColor));
	shader->SendFlaot3("u_Lighting._SpotLights[" + id + "].Light.Light.DiffuseColor",	glm::value_ptr(m_DiffuseColor));
	shader->SendFlaot3("u_Lighting._SpotLights[" + id + "].Light.Light.SpecularColor",	glm::value_ptr(m_SpecularColor));
	shader->SendFlaot3("u_Lighting._SpotLights[" + id + "].Light.Light.Direction",		glm::value_ptr(m_Direction));
	shader->SendFlaot3("u_Lighting._SpotLights[" + id + "].Light.Position",				glm::value_ptr(m_Position));
	shader->SendFlaot("u_Lighting._SpotLights[" + id + "].Light.Constant",               m_Constant);
	shader->SendFlaot("u_Lighting._SpotLights[" + id + "].Light.Linear",                 m_Linear);
	shader->SendFlaot("u_Lighting._SpotLights[" + id + "].Light.Qaudratic",              m_Qaudratic);
	shader->SendFlaot("u_Lighting._SpotLights[" + id + "].MinAngle",                     m_MinAngle);
	shader->SendFlaot("u_Lighting._SpotLights[" + id + "].MaxAngle",                     m_MaxAngle);

	if (m_Id == m_Count - 1)
		m_Id = 0;
	else
		m_Id++;
}