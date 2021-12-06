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
	if (m_pDocker != nullptr && m_EntityHandle != entt::null)
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

std::vector<shade::Entity>& shade::Entity::GetChildren()
{
	return GetComponent<PCComponent>().Children;
}

void shade::Entity::AddChild(shade::Entity& entity)
{
	entity.SetParent(*this);
	GetComponent<PCComponent>().Children.push_back(entity);
}

void shade::Entity::RemoveChild(shade::Entity& entity)
{
	//entity.m_EntityHandle
	auto& children = GetComponent<PCComponent>().Children;
 	auto it = std::find(children.begin(), children.end(), entity);
	if (it != children.end())
	{
		entity.UnsetParent();
		children.erase(it);
	}
}

bool shade::Entity::HasChildren()
{
	return GetComponent<PCComponent>().Children.size();
}

shade::Entity shade::Entity::GetParent()
{
	return Entity((entt::entity)GetComponent<PCComponent>().Parent, m_pDocker);
}

void shade::Entity::SetParent(shade::Entity& entity)
{
	GetComponent<PCComponent>().Parent = entity;
}

void shade::Entity::UnsetParent()
{
	GetComponent<PCComponent>().Parent = shade::Entity();
}
