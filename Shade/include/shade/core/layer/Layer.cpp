#include "shade_pch.h"
#include "Layer.h"

shade::Layer::Layer(const std::string& name) :
    m_Name(name)
{
}

shade::Layer::~Layer()
{
}

void shade::Layer::SetActive(const bool& isActive)
{
    m_IsActive = isActive;
}

void shade::Layer::SetRender(const bool& isRender)
{
    m_IsRender = isRender;
}

void shade::Layer::SetUpdate(const bool& isUpdate)
{
    m_IsUpdate = isUpdate;
}

const bool& shade::Layer::IsActive() const
{
    return m_IsActive;
}

const bool& shade::Layer::IsRender() const
{
    return m_IsRender;
}

const bool& shade::Layer::IsUpdate() const
{
    return m_IsUpdate;
}

const std::string& shade::Layer::GetName() const
{
    return m_Name;
}
