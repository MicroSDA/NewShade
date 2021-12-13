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

bool shade::Entity::RemoveChild(shade::Entity& entity)
{
	auto& children = GetComponent<PCComponent>().Children;
 	auto it = std::find(children.begin(), children.end(), entity);
	if (it != children.end())
	{
		it->UnsetParent(true);
		children.erase(it);
		return true;
	}
	return false;
}

bool shade::Entity::HasChildren() const
{
	return GetComponent<PCComponent>().Children.size();
}

shade::Entity& shade::Entity::GetParent()
{
	return GetComponent<PCComponent>().Parent;
}

bool shade::Entity::AddChild(shade::Entity& entity)
{
	if (entity != *this)
	{
		if (!IsChildOf(entity))
		{
			entity.SetParent(*this);
			GetComponent<PCComponent>().Children.push_back(entity);
			return true;
		}
	}
	else
	{
		SHADE_CORE_ERROR("Couldn't add child, parent and child are the same!");
		return false;
	}
}

bool shade::Entity::IsChildOf(shade::Entity& entity)
{
	if (HasParent())
	{
		if (GetParent() == entity)
			return true;
		
		if (GetParent().IsChildOf(entity))
			return true;
	}

	return false;
}

void shade::Entity::SetParent(shade::Entity& entity)
{
	if (HasParent())
	{
		GetParent().RemoveChild(*this);
	}

	GetComponent<PCComponent>().Parent = entity;
}

void shade::Entity::UnsetParent(const bool& isRecursive)
{
	if (HasParent() && !isRecursive)
	{
		GetParent().RemoveChild(*this);
	}

	GetComponent<PCComponent>().Parent = shade::Entity();
}

bool shade::Entity::HasParent() const
{
	return GetComponent<PCComponent>().Parent.IsValid();
}
