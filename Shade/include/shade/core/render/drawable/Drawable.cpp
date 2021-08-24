#include "shade_pch.h"
#include "Drawable.h"

void shade::Drawable::GenerateHalfExt()
{
    // Generate min and max half extension based on mesh origin
    float xMax = 0.0f, yMax = 0.0f, zMax = 0.0f;
    unsigned int ixMax = 0, iyMax, izMax = 0, jxMax = 0, jyMax, jzMax = 0;

    for (auto i = 0; i < GetVertices().size(); i++)
    {
        for (auto j = i + 1; j < GetVertices().size(); j++)
        {
            auto x = std::abs(GetVertices()[j].Position.x - GetVertices()[i].Position.x);
            if (x > xMax)
            {
                xMax = x; ixMax = i; jxMax = j;
            }

            auto y = std::abs(GetVertices()[j].Position.y - GetVertices()[i].Position.y);
            if (y > yMax)
            {
                yMax = y;  iyMax = i; jyMax = j;
            }
          
            auto z = std::abs(GetVertices()[j].Position.z - GetVertices()[i].Position.z);
            if (z > zMax)
            {
                zMax = z; izMax = i; jzMax = j;
            }
           
        }
    }

    // Some optimization 
    m_MinHalfExt = glm::vec3(std::min(GetVertices()[ixMax].Position.x, GetVertices()[jxMax].Position.x),
                             std::min(GetVertices()[iyMax].Position.y, GetVertices()[jyMax].Position.y),
                             std::min(GetVertices()[izMax].Position.z, GetVertices()[jzMax].Position.z));

    m_MaxHalfExt = glm::vec3(std::max(GetVertices()[ixMax].Position.x, GetVertices()[jxMax].Position.x),
                             std::max(GetVertices()[iyMax].Position.y, GetVertices()[jyMax].Position.y),
                             std::max(GetVertices()[izMax].Position.z, GetVertices()[jzMax].Position.z));
}

const glm::vec3& shade::Drawable::GetMinHalfExt()
{
    return m_MinHalfExt;
}

const glm::vec3& shade::Drawable::GetMaxHalfExt()
{
    return m_MaxHalfExt;
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
