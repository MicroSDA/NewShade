#include "shade_pch.h"
#include "Camera.h"
#include "shade/core/window/Window.h"

shade::Camera::Camera() :
	m_Fov(45.0f),  m_Aspect(1), // Be aware about aspect 1
	m_zNear(0.1f), m_zFar(3000.0f)
{
	m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
	m_Position = glm::vec3(0, 0, -10);
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

	m_Perpective = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	m_Position = position;
	m_Forward = glm::vec3(0, 0, 1); // - Z
	m_Up = glm::vec3(0, 1, 0);
}

void shade::Camera::Resize(const float& aspect)
{
	if (aspect)
	{
		m_Aspect = aspect;
		m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
	}
	else
	{
		m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
	}

	//m_Aspect = (float)shade::WindowManager::GetWindow().Width / (float)shade::WindowManager::GetWindow().Height;

//m_Perpective = glm::perspective(glm::radians(m_Fov), m_Aspect, m_zNear, m_zFar);
//TODO Zoom
}

void shade::Camera::SetPrimary(const bool& isPrimary)
{
	m_IsPrimary = isPrimary;
}
