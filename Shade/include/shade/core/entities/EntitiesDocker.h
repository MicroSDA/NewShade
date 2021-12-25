#pragma once
#include "shade/config/API.h"
#include <entt/entt.hpp>

namespace shade
{
	class Entity;

	class SHADE_API EntitiesDocker
	{
	public:
		EntitiesDocker() = default;
		virtual ~EntitiesDocker();

		shade::Entity CreateEntity(const std::string& name = "Entity");
	
		void DestroyEntity(Entity& entity);
		void DestroyEntities();
		const entt::registry& GetEntities() const;
		entt::registry& GetEntities();
		shade::Entity   GetEntity(const std::uint32_t& id);
	private:
		entt::registry     m_Registry;
	};
}

