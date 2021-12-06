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
	private:
		entt::registry     m_Registry;
	};
}

