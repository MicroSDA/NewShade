#pragma once
#include "shade/config/API.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/utils/Log.h"
#include <entt/entt.hpp>

namespace shade
{
	class SHADE_API Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, shade::EntitiesDocker* docker);
		Entity(const Entity& other) : 
			m_EntityHandle(other.m_EntityHandle),
			m_pDocker(other.m_pDocker)
		{ }
		~Entity();
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			if (HasComponent<T>())
				SHADE_CORE_ERROR("Entity already has component: '{0}'", typeid(T).name());
			return m_pDocker->GetEntities().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}
		template<typename T>
		T& GetComponent() const
		{
			if (!HasComponent<T>())
				SHADE_CORE_ERROR("Entity does not have component: '{0}'", typeid(T).name());
			return m_pDocker->GetEntities().get<T>(m_EntityHandle);
		}
		template<typename T>
		void RemoveComponent()
		{
			if (!HasComponent<T>())
				SHADE_CORE_ERROR("Entity does not have component: '{0}'", typeid(T).name());
			m_pDocker->GetEntities().remove<T>(m_EntityHandle);
		}
		template<typename T>
		bool HasComponent() const
		{
			return m_pDocker->GetEntities().has<T>(m_EntityHandle);
		}
		/*template<typename...T>
		bool HasAnyOfComponents() const
		{
			return m_pDocker->GetEntities().any_of<T...>(m_EntityHandle);
		}*/

		bool IsValid();
		void Destroy();
		shade::EntitiesDocker& GetDocker();
		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator uint64_t() const { return (uint64_t)m_EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_pDocker == other.m_pDocker;
		}
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
		friend std::string operator+(const std::string& string, const Entity& other)
		{
			return std::string(string + std::to_string(static_cast<uint32_t>(other.m_EntityHandle)));
		}
		operator std::string () { return std::to_string(static_cast<uint32_t>(m_EntityHandle)); }
		operator const std::string () const { return std::to_string(static_cast<uint32_t>(m_EntityHandle)); }

		std::vector<shade::Entity>& GetChildren();
		void AddChild(shade::Entity& entity);
		void RemoveChild(shade::Entity& entity);
		bool HasChildren();

		void SetParent(shade::Entity& entity);
		void UnsetParent();
		shade::Entity& GetParent();
	private:
		entt::entity m_EntityHandle { entt::null };
		shade::EntitiesDocker*		m_pDocker = nullptr;
	};

	// Parent - Child component
	struct PCComponent
	{
		shade::Entity Parent;
		std::vector<shade::Entity> Children;

		PCComponent() = default;
		PCComponent(const PCComponent& other) = default;
		PCComponent(shade::Entity parent)
			: Parent(parent)
		{}

	};
}
