#include "shade_pch.h"
#include "Transform3D.h"

shade::Transform3D::Transform3D():
	m_Possition(0.0f, 0.0f, 0.0f),
	m_Rotation(0.0f, 0.0f, 0.0f),
	m_Scale(1.0f, 1.0f, 1.0f)
{

}

shade::Transform3D::Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale):
	m_Possition(position), m_Rotation(rotation), m_Scale(scale)
{ }

glm::mat4 shade::Transform3D::GetModelMatrix() const
{
	return glm::translate(glm::mat4(1.0f), m_Possition) * glm::toMat4(glm::quat((m_Rotation))) * glm::scale(glm::mat4(1.0f), m_Scale);
}
