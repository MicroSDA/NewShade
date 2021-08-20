#include "shade_pch.h"
#include "AssetData.h"

shade::AssetData::AssetData():
    m_IsValid(false)
{
    m_AssetData = m_Root.append_child("asset");
    m_AssetData.append_attribute("Id");
    m_AssetData.append_attribute("Description");
    m_AssetData.append_attribute("Category");
    m_AssetData.append_attribute("Type");
    m_AssetData.append_attribute("Path");
}

shade::AssetData::AssetData(pugi::xml_node data):
    m_AssetData(data), m_IsValid(true)
{
}

void shade::AssetData::Append(AssetData& data)
{
    if (m_AssetData.child("dependencies").empty())
    {
        m_AssetData.append_child("dependencies");
    }

    data = m_AssetData.child("dependencies").append_copy(data._Raw());
}

const bool& shade::AssetData::IsValid() const
{
    return m_IsValid;
}

shade::AssetData shade::AssetData::Parent()
{
    return AssetData();
}

std::vector<shade::AssetData> shade::AssetData::Dependencies() const
{
    std::vector<AssetData> dependencies;
    //dependencies.reserve(m_AssetData.child("dependencies").attribute("count").as_int());
    // &dependency ?
    if (!m_AssetData.child("dependencies").empty())
    {
        for (auto dependency : m_AssetData.child("dependencies").children("dependency"))
        {
            dependencies.push_back(dependency);
        }
    }
    
    return dependencies;
}

pugi::xml_attribute shade::AssetData::Attribute(const std::string& attr) const
{
    return m_AssetData.attribute(attr.c_str());
}

pugi::xml_node shade::AssetData::_Raw() const
{
    return m_AssetData;
}

bool shade::AssetData::Serialize(std::ostream& stream) const
{
    return false;
}

bool shade::AssetData::Deserialize(std::istream& stream)
{
    return false;
}

bool shade::AssetData::Serialize() const
{
    std::string path = Attribute("Path").as_string();

    if (path.empty())
    {
        SHADE_CORE_WARNING("Saving path is empty for asset data '{0}'", Attribute("Id").as_string());
        return false;
    }

    path = path + Attribute("Id").as_string();
    path += ".xml";
    pugi::xml_document file;
    file.append_copy(_Raw());
    file.save_file(path.c_str());
    // Need to check if file was saved successful !
    return true;
}

bool shade::AssetData::Deserialize()
{
    return false;
}
