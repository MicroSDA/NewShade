#include "shade_pch.h"
#include "Entity.h"

shade::Entity::Entity(entt::entity handle, shade::EntitiesDocker* docker) :
	m_EntityHandle(handle), m_pDocker(docker)
{
}

shade::Entity::~Entity()
{
}

bool shade::Entity::IsValid()
{
	if (m_pDocker != nullptr)
		return m_pDocker->GetEntities().valid(m_EntityHandle);
	else return false;
}

void shade::Entity::Destroy()
{
	m_pDocker->DestroyEntity(*this);
}

shade::EntitiesDocker& shade::Entity::GetDocker()
{
	return *m_pDocker;
}