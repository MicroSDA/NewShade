#pragma once
#include "shade/config/API.h"
#include "shade/core/serializing/Serializable.h"
#include "shade/core/assets/AssetData.h"
namespace shade
{
	class SHADE_API Asset : public Serializable
	{
	public:
		enum class State
		{
			KeepAlive,
			RemoveIfPosible
		};
	public:
		Asset() = default;
		virtual ~Asset();
		virtual void LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle = AssetData()) = 0;
		void SetAssetData(shade::AssetData& data);
		AssetData& GetAssetData();
		const AssetData& GetAssetData() const;
		virtual void AssetInit() = 0;
	private:
		AssetData  m_AssetData;
	};
}