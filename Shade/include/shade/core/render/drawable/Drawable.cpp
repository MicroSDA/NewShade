#include "shade_pch.h"
#include "Drawable.h"

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

void shade::Drawable::SetMaterial(const Material& material)
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

const shade::Material& shade::Drawable::GetMaterial() const
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

shade::Material& shade::Drawable::GetMaterial()
{
    return const_cast<Material&>(const_cast<const shade::Drawable*>(this)->GetMaterial());
}
