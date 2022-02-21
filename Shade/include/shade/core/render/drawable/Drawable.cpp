#include "shade_pch.h"
#include "Drawable.h"

void shade::Drawable::GenerateHalfExt()
{
    //// Generate min and max half extension based on mesh origin
    m_MinHalfExt  = { FLT_MAX, FLT_MAX, FLT_MAX };
    m_MaxHalfExt  = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    auto& vertices = GetVertices();
    for (size_t i = 0; i < vertices.size(); i++)
    {
        m_MinHalfExt.x = glm::min(vertices[i].Position.x, m_MinHalfExt.x);
        m_MinHalfExt.y = glm::min(vertices[i].Position.y, m_MinHalfExt.y);
        m_MinHalfExt.z = glm::min(vertices[i].Position.z, m_MinHalfExt.z);
        m_MaxHalfExt.x = glm::max(vertices[i].Position.x, m_MaxHalfExt.x);
        m_MaxHalfExt.y = glm::max(vertices[i].Position.y, m_MaxHalfExt.y);
        m_MaxHalfExt.z = glm::max(vertices[i].Position.z, m_MaxHalfExt.z);
    }
}

const glm::vec3& shade::Drawable::GetMinHalfExt() const
{
    return m_MinHalfExt;
}

const glm::vec3& shade::Drawable::GetMaxHalfExt() const 
{
    return m_MaxHalfExt;
}

void shade::Drawable::SetMinHalfExt(const glm::vec3& ext)
{
    m_MinHalfExt = ext;
}

void shade::Drawable::SetMaxHalfExt(const glm::vec3& ext)
{
    m_MaxHalfExt = ext;
}

const shade::Drawable::DrawMode& shade::Drawable::GetDrawMode() const
{
    return m_DrawMode;
}

void shade::Drawable::SetDrawMode(const DrawMode& mode)
{
    m_DrawMode = mode;
}

void shade::Drawable::AddVertex(const Vertex3D& vertex)
{
    m_Vertices.push_back(vertex);
}

void shade::Drawable::AddIndex(const Index& index)
{
    m_Indices.push_back(index);
}

void shade::Drawable::AddVertices(Vertices& vertices)
{
    // Not sure about it, need test
    m_Vertices.resize(m_Vertices.size() + vertices.size(), *vertices.data());
}

void shade::Drawable::AddIndices(Indices& indices)
{
    // Not sure about it, need test
    m_Indices.resize(m_Indices.size() + indices.size(), *indices.data());
}

void shade::Drawable::SetVertices(Vertices& vertices)
{
    m_Vertices = std::move(vertices);
}

void shade::Drawable::SetIndices(Indices& indices)
{
    m_Indices = std::move(indices);
}

void shade::Drawable::SetMaterial(const Shared<Material3D>& material)
{
    m_Material = material;
}

const shade::Vertices& shade::Drawable::GetVertices() const
{
    return m_Vertices;
}

const shade::Indices& shade::Drawable::GetIndices() const
{
    return m_Indices;
}

const shade::Shared<shade::Material3D>& shade::Drawable::GetMaterial() const
{
    return m_Material;
}

shade::Vertices& shade::Drawable::GetVertices()
{
    return const_cast<Vertices&>(const_cast<const shade::Drawable*>(this)->GetVertices());
}

shade::Indices& shade::Drawable::GetIndices()
{
    return const_cast<Indices&>(const_cast<const shade::Drawable*>(this)->GetIndices());
}

shade::Shared<shade::Material3D>& shade::Drawable::GetMaterial()
{
    return const_cast<Shared<Material3D>&>(const_cast<const shade::Drawable*>(this)->GetMaterial());
}
