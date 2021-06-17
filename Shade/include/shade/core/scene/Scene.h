#pragma once
#include "shade/config/API.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/components/Components.h"
#include "shade/core/time/Timer.h"

namespace shade
{
	class SHADE_API Scene : public EntitiesDocker
	{
	public:
		Scene(const std::string& name);
		virtual ~Scene();
		void NativeScriptsUpdate(const shade::Timer& deltaTime);
		const std::string& GetName() const;
		Entity GetPrimaryCamera();
	private:
		std::string m_Name;
	};
}