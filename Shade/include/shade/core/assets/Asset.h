#pragma once
#include "shade/config/API.h"
#include "shade/core/serializing/Serializable.h"
#include "shade/core/assets/AssetData.h"
namespace shade
{
	class SHADE_API Asset : public Serializable
	{
	public:
		enum class Lifetime
		{
			KeepAlive,
			Destroy
		};
	public:
		Asset();
		virtual ~Asset();
		virtual void LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle = AssetData()) = 0;
		void SetAssetData(shade::AssetData& data);
		AssetData& GetAssetData();
		const AssetData& GetAssetData() const;
		const bool& IsPrefab() const;
		virtual void AssetInit() = 0;
	private:
		friend class AssetManager;

		AssetData  m_AssetData;
		bool	   m_IsPrefab;

		void _SetPrefab(const bool& isPrefab);
	};
}