#include "shade_pch.h"
#include "Camera.h"
#include "shade/core/window/Window.h"

shade::Camera::Camera() :
	m_Fov(45.0f),  m_Aspect(1), // Be aware about aspect 1
	m_zNear(0.1f), m_zFar(1000.0f)
{
	_RecalculatePerpective();

	m_Position = glm::vec3(0, 0, - 10);
	m_Forward = glm::vec3(0, 0, 1); // - Z
	m_Up = glm::vec3(0, 1, 0);

}
shade::Camera::Camera(const glm::vec3& position,
	const float& fov,
	const float& aspect,
	const float& zNear,
	const float& zFar)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;

	m_Perpective	= glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	m_Position		= position;
	m_Forward		= glm::vec3(0, 0, 1);
	m_Up			= glm::vec3(0, 1, 0);
}

void shade::Camera::RotateY(const float& angle)
{
	/* With counterclockwise issue */
	//glm::mat4 rotation = glm::rotate(angle, UP);
	/* Without counterclockwise issue */
	glm::mat4 rotation  = glm::rotate(angle , glm::dot(UP, m_Up) < 0.f? - UP: UP);
	m_Forward			= glm::vec3(glm::normalize(rotation * glm::vec4(m_Forward, 0.f)));
	m_Up				= glm::vec3(glm::normalize(rotation * glm::vec4(m_Up, 0.f)));
}

void shade::Camera::RotateX(const float& angle)
{
	glm::vec3 right = glm::normalize(glm::cross(m_Up,		m_Forward));
	m_Forward		= glm::normalize(glm::rotate(-angle,	right) * glm::vec4(m_Forward, 0.f));
	m_Up			= glm::normalize(glm::cross(m_Forward,	right));	
}


void shade::Camera::RotateZ(const float& angle)
{
	m_Up = glm::normalize(glm::rotate(-angle, m_Forward) * glm::vec4(m_Up, 0.f));
}

void shade::Camera::Rotate(const glm::vec3& angle)
{
	RotateY(-angle.x);
	RotateX(-angle.y);
	RotateZ(angle.z);
}
// TODO maby need to set viewProjection and recalculate it, check !
void shade::Camera::SetVeiw(const glm::mat4& view)
{
	glm::mat4 matrix	= GetProjection() * glm::inverse(view);
	//glm::vec2 right	= glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]);
	m_Forward			= glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]);
	m_Up				= glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]);
	m_Position			= glm::vec3(view[3][0], view[3][1], view[3][2]);
}

void shade::Camera::Resize(const float& aspect)
{
	// TODO Zoom
	// TODO m_Fov has to be in radians ?
	if (aspect)
	{
		m_Aspect = aspect;
		m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
	}
	else
		m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
}

void shade::Camera::SetAsPrimary(const bool& isPrimary)
{
	m_IsPrimary = isPrimary;
}

void shade::Camera::_RecalculatePerpective()
{
	m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
}
