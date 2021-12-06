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
		template<typename T, typename ...Args>
		static Shared<T> Create(Args&& ...args);

		Asset();
		virtual ~Asset();
		void SetAssetData(shade::AssetData& data);
		AssetData& GetAssetData();
		const AssetData& GetAssetData() const;
		const bool& IsPrefab() const;
		virtual void AssetInit() = 0;
	protected:
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) = 0;
	private:
		friend class AssetManager;
		// Acces only in AssetManager
		void _SetPrefab(const bool& isPrefab);
		void _LoadFromAssetData(shade::AssetData& data);


		AssetData  m_AssetData;
		bool	   m_IsPrefab;
	};
	template<typename T, typename ...Args>
	inline static Shared<T> Asset::Create(Args&& ...args)
	{
		return CreateShared<T>(std::forward<Args>(args)...);
	}
}
