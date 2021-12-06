#include "shade_pch.h"
#include "EntitiesDocker.h"

#include "shade/core/entities/Entity.h"

shade::EntitiesDocker::~EntitiesDocker()
{
    GetEntities().clear();
}

shade::Entity shade::EntitiesDocker::CreateEntity(const std::string& name)
{
    //Проверить статичную линковку и старую версию ентт
    shade::Entity entity = {m_Registry.create(), this};
    auto& tag = entity.AddComponent<shade::Tag>();
    entity.AddComponent<PCComponent>();
    tag = (name == "Entity") ? "Entity" : name;
    return entity;
}
void shade::EntitiesDocker::DestroyEntity(const Entity& entity)
{
    m_Registry.destroy(entity);
}

void shade::EntitiesDocker::DestroyEntities()
{
    m_Registry.clear();
}

const entt::registry& shade::EntitiesDocker::GetEntities() const
{
    return m_Registry;
}

entt::registry& shade::EntitiesDocker::GetEntities()
{
    return m_Registry;
}
