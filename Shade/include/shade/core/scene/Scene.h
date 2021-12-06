#pragma once
#include "shade/config/API.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/components/Components.h"
#include "shade/core/time/Timer.h"
#include "shade/core/assets/Asset.h"

namespace shade
{
	class SHADE_API Scene : public EntitiesDocker, public Asset
	{
	public:
		static Shared<Scene> Create();
		virtual ~Scene();
		void NativeScriptsUpdate(const shade::Timer& deltaTime);
		const std::string& GetName() const;
		Entity GetPrimaryCamera();

		void OnPlayStart();
		void OnPlaying(const shade::Timer& deltaTime);
		void OnPlayStop();

		const bool& IsPlaying() const;
		void SetPlaying(const bool& play);

		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
		virtual void AssetInit() override;

	private:
		Scene();
		std::string m_Name;
		bool        m_IsPlaying;

		// Asset loading behaviour
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) override;
	};
}