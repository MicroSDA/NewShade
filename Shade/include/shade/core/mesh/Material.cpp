#include "shade_pch.h"
#include "Material.h"

shade::Material::Material() :
    m_AmbientColor(0.1f, 0.1f, 0.1f),
    m_DiffuseColor(1.0f, 1.0f, 1.0f),
    m_SpecularColor(1.0f, 1.0f, 1.0f),
    m_TransparentMask(0.0f, 0.0f, 0.0f),
    m_Shininess(1),
    m_ShininessStrength(10)
{
}

shade::Material::~Material()
{
}

void shade::Material::SetName(const std::string& name)
{
    m_Name = name;
}

void shade::Material::SetAmbientColor(const float& r, const float& g, const float& b)
{
    m_AmbientColor = glm::vec3(r, g, b);
}

void shade::Material::SetAmbientColor(const glm::vec3& color)
{
    m_AmbientColor = color;
}

void shade::Material::SetDiffuseColor(const float& r, const float& g, const float& b)
{
    m_DiffuseColor = glm::vec3(r, g, b);
}

void shade::Material::SetDiffuseColor(const glm::vec3& color)
{
    m_DiffuseColor = color;
}

void shade::Material::SetSpecularColor(const float& r, const float& g, const float& b)
{
    m_SpecularColor = glm::vec3(r, g, b);
}

void shade::Material::SetSpecularColor(const glm::vec3& color)
{
    m_SpecularColor = color;
}

void shade::Material::SetTransparentMask(const float& r, const float& g, const float& b)
{
    m_TransparentMask = glm::vec3(r, g, b);
}

void shade::Material::SetTransparentMask(const glm::vec3& color)
{
    m_TransparentMask = color;
}

void shade::Material::SetShininess(const float& value)
{
    m_Shininess = value;
}

void shade::Material::SetShininessStrength(const float& value)
{
    m_ShininessStrength = value;
}

const std::string& shade::Material::GetName() const
{
    return m_Name;
}

const glm::vec3& shade::Material::GetAmbientColor() const
{
    return m_AmbientColor;
}

glm::vec3& shade::Material::GetAmbientColor()
{
    return const_cast<glm::vec3&>(const_cast<const shade::Material*>(this)->GetAmbientColor());
}

const glm::vec3& shade::Material::GetDiffuseColor() const
{
    return m_DiffuseColor;
}

glm::vec3& shade::Material::GetDiffuseColor()
{
    return const_cast<glm::vec3&>(const_cast<const shade::Material*>(this)->GetDiffuseColor());
}

const glm::vec3& shade::Material::GetSpecularColor() const
{
    return m_SpecularColor;
}

glm::vec3& shade::Material::GetSpecularColor()
{
    return const_cast<glm::vec3&>(const_cast<const shade::Material*>(this)->GetSpecularColor());
}

const glm::vec3& shade::Material::GetTransparentMask() const
{
    return m_TransparentMask;
}

glm::vec3& shade::Material::GetTransparentMask()
{
    return const_cast<glm::vec3&>(const_cast<const shade::Material*>(this)->GetTransparentMask());
}

const float& shade::Material::GetShininess() const
{
    return m_Shininess;
}

float& shade::Material::GetShininess()
{
    return const_cast<float&>(const_cast<const shade::Material*>(this)->GetShininess());
}

const float& shade::Material::GetShininessStrength() const
{
    return m_ShininessStrength;
}

float& shade::Material::GetShininessStrength()
{
    return const_cast<float&>(const_cast<const shade::Material*>(this)->GetShininessStrength());
}

void shade::Material::Process(const Shared<Shader>& shader) const
{
    shader->SendFlaot3("u_Material.AmbientColor",     glm::value_ptr(m_AmbientColor));
    shader->SendFlaot3("u_Material.DiffuseColor",     glm::value_ptr(m_DiffuseColor));
    shader->SendFlaot3("u_Material.SpecularColor",    glm::value_ptr(m_SpecularColor));
    shader->SendFlaot3("u_Material.transparentMask",  glm::value_ptr(m_TransparentMask));
    shader->SendFlaot("u_Material.Shinines",          m_Shininess);
    shader->SendFlaot("u_Material.ShininesStrength",  m_ShininessStrength);
}