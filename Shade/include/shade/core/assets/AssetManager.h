#pragma once
#include "shade/config/API.h"
#include "shade/core/utils/threads/ThreadPool.h"
#include "shade/core/assets/Asset.h"
#include "shade/core/Types.h"

namespace shade
{
	// For loading and manage asset data
	class SHADE_API AssetManager
	{
	public:
		// Contain references between asset and holder/receiver
		struct AssetReference
		{
			std::variant<shade::Weak<shade::Asset>, shade::Shared<shade::Asset>> Reference;
		};
		// Book of all assets and their references
		using AssetReferences = std::unordered_map <std::string, AssetReference>;
		// Contain asset metadata required for loading and managment
		using AssetsDataList = std::unordered_map <std::string, AssetData>;
		// Function for dispatching asset ot receivers when loading task is done
		using ReceiverCallback = std::function<void(shade::Shared<shade::Asset>&)>;
		using PostLoadCallback = std::function<void(shade::Shared<shade::Asset>&)>;
		// Contain a loading result
		using LoadingResult = std::future<std::tuple<Shared<Asset>, std::exception_ptr>>;
	public:
		friend class Application;
		friend class Asset;
		static bool LoadAssetDataListFromFile(const std::string& filePath);
		// Use for loading new asset into memeory by specific asset id.
		// If you want to kepp an asset in memeory even if all holders was deleted, 
		// use Asset::Lifetime::KeepAlive or Asset::Lifetime::Destroy if you don't want to keep it in memeory.
		// All assets that were held with Asset::Lifetime::Destroy flag will be removed automatically !
		template<typename T>
		static void HoldAsset(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime = Asset::Lifetime::Destroy);
		// Use for loading new prefab into memeory by specific asset id.
		// If you want to kepp an prefab in memeory even if all holders was deleted, 
		// use Asset::Lifetime::KeepAlive or Asset::Lifetime::Destroy if you don't want to keep it in memeory.
		// All prefabs that were held with Asset::Lifetime::Destroy flag will be removed automatically !
		template<typename T>
		static void HoldPrefab(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime = Asset::Lifetime::Destroy);

		// For casting asset
		template<typename T>
		static auto Receive(shade::Shared<shade::Asset>& asset)->auto;
		// Unload specific asset if ref count is 0
		static void Free(const std::string& id);
		/* Getting all assets */
		static AssetsDataList& GetAssetsDataList();
		/* Getting specific asset by asset id*/
		static AssetData GetAssetData(const std::string& id);
		/* Getting all prefabs */
		static AssetsDataList& GetPrefabsDataList();
		/* Getting specific prefab by asset id*/
		static AssetData GetPrefabsData(const std::string& id);
	public:
		~AssetManager();
	private:
		struct AssetLoadingTask
		{
			AssetLoadingTask() = default;
			AssetLoadingTask(const std::string& id, const Asset::Lifetime& state, LoadingResult& result, ReceiverCallback callback) :
				Id(id), State(state), Result(std::move(result))
			{
				ReceiverCallbacks.push_back(callback);
			}
			std::string						Id;
			Asset::Lifetime					State = Asset::Lifetime::Destroy;
			LoadingResult					Result;
			std::vector<ReceiverCallback>	ReceiverCallbacks;
		};
	private:
		//Singleton implementation
		AssetManager();
		AssetManager(const AssetManager&) = delete;
		AssetManager& operator= (const AssetManager&) = delete;
		AssetManager(const AssetManager&&) = delete;
		AssetManager& operator= (const AssetManager&&) = delete;
	private:
		static AssetManager& _Get();
		/* Reading prefarbs form asset data list*/
		void _RecursiveReadingPrefabsDataList(pugi::xml_node node);
		/* Reading assets form asset data list*/
		void _ReadAssetDataList(pugi::xml_node node);
		/* Load new asset */
		template<typename T>
		void _LoadNewAsset(const std::string& id,  PostLoadCallback callback, const Asset::Lifetime& lifeTime);
		/* Load new asset as prefab*/
		template<typename T>
		void _LoadNewPrefab(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime);
		/* Internal function for dispath assets to recivers */
		void _DispatchAssets(std::map<const std::string, AssetLoadingTask>& taskList, shade::AssetManager::AssetReferences& references, std::mutex& mutex);
		/* Dispath assets, using by Application only*/
		void _Dispatch();
		// Internal use for asset object only, will remove entry from AssetsDataList
		static void _ImLast(const std::string& assetId, const bool& isPrefab);
		// Internal use for Application only
		static void _Clear();
	
		pugi::xml_document								m_AssetsDoc;
		shade::util::ThreadPool							m_TrheadPool;

		bool											m_ImDestructing = false;
		std::mutex										m_AssetsTaskMutex;
		std::mutex										m_PrefabsTaskMutex;

		AssetsDataList									m_AssetsDataList;
		AssetsDataList									m_PrefabDataList;

		AssetReferences									m_AssetReferences;
		AssetReferences									m_PrefabReferences;
		
		// Why there is not unordered map !!
		std::map<const std::string, AssetLoadingTask>	m_AssetsTaskList;
		std::map<const std::string, AssetLoadingTask>	m_PrefabsTaskList;
	};
	template<typename T>
	inline void AssetManager::HoldAsset(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime)
	{
		// Check if tsk exist at first!!!!!!!!!! Issue!!!!!!!
		auto& manager = _Get();
		std::unique_lock<std::mutex> lock{ manager.m_AssetsTaskMutex };
		// Trying to find in Assets data list
		auto assetReference = manager.m_AssetReferences.find(id);
		if (assetReference != manager.m_AssetReferences.end())
		{
			// Allready loaded
			// Check if asset was loaded with State::RemoveIfPosible
			if (std::get_if<shade::Weak<shade::Asset>>(&assetReference->second.Reference))
			{
				auto& weakAssetReference = std::get<shade::Weak<shade::Asset>>(assetReference->second.Reference);
				// If nobody links
				if (!weakAssetReference.expired())
				{   // Then asset has been removed
					shade::Shared<shade::Asset> assetPtr(std::get<shade::Weak<shade::Asset>>(assetReference->second.Reference).lock());

					auto task = manager.m_AssetsTaskList.find(id);

					if (task != manager.m_AssetsTaskList.end())
					{
						manager.m_AssetsTaskList[id].ReceiverCallbacks.push_back(callback);
					}
					else
					{
						// Create result for loading task
						std::promise<std::tuple<Shared<Asset>, std::exception_ptr>>  promise;
						std::future<std::tuple<Shared<Asset>, std::exception_ptr>>  result(promise.get_future());
						promise.set_value({ assetPtr, nullptr });
						//////////////////////
						AssetLoadingTask task(id, lifeTime, result, callback);
						manager.m_AssetsTaskList[id] = std::move(task);
					}
				}
				else
				{
					// Start to load new asset
					manager._LoadNewAsset<T>(id, callback, lifeTime);
				}
			} // Check if asset was loaded with State::KeepAlive
			else if (std::get_if<shade::Shared<shade::Asset>>(&assetReference->second.Reference))
			{
				shade::Shared<shade::Asset> assetPtr(std::get<shade::Shared<shade::Asset>>(assetReference->second.Reference));
				// Create result for loading task
				std::promise<std::tuple<Shared<Asset>, std::exception_ptr>>  promise;
				std::future<std::tuple<Shared<Asset>, std::exception_ptr>>   result(promise.get_future());
				promise.set_value({ assetPtr, nullptr });
				//////////////////////
				AssetLoadingTask task(id, lifeTime, result, callback);
				manager.m_AssetsTaskList[id] = std::move(task);
			}

		}// We couldn't find so need to load
		else
		{
			manager._LoadNewAsset<T>(id, callback, lifeTime);
		}
	}

	template<typename T>
	inline void AssetManager::HoldPrefab(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime)
	{
		// Check if tsk exist at first!!!!!!!!!! Issue !!!!!!!
		auto& manager = _Get();
		std::unique_lock<std::mutex> lock{ manager.m_PrefabsTaskMutex };
		// Trying to find in Assets data list
		auto assetReference = manager.m_PrefabReferences.find(id);
		if (assetReference != manager.m_PrefabReferences.end())
		{
			// Allready loaded
			// Check if asset was loaded with State::RemoveIfPosible
			if (std::get_if<shade::Weak<shade::Asset>>(&assetReference->second.Reference))
			{
				auto& weakAssetReference = std::get<shade::Weak<shade::Asset>>(assetReference->second.Reference);
				// If nobody links
				if (!weakAssetReference.expired())
				{   // Then asset has been removed
					shade::Shared<shade::Asset> assetPtr(std::get<shade::Weak<shade::Asset>>(assetReference->second.Reference).lock());

					auto task = manager.m_PrefabsTaskList.find(id);

					if (task != manager.m_PrefabsTaskList.end())
					{
						manager.m_PrefabsTaskList[id].ReceiverCallbacks.push_back(callback);
					}
					else
					{
						// Create result for loading task
						std::promise<std::tuple<Shared<Asset>, std::exception_ptr>>  promise;
						std::future<std::tuple<Shared<Asset>, std::exception_ptr>>  result(promise.get_future());
						promise.set_value({ assetPtr, nullptr });
						//////////////////////
						AssetLoadingTask task(id, lifeTime, result, callback);
						manager.m_PrefabsTaskList[id] = std::move(task);
					}
				}
				else
				{
					// Start to load new asset
					manager._LoadNewPrefab<T>(id, callback, lifeTime);
				}
			} // Check if asset was loaded with State::KeepAlive
			else if (std::get_if<shade::Shared<shade::Asset>>(&assetReference->second.Reference))
			{
				shade::Shared<shade::Asset> assetPtr(std::get<shade::Shared<shade::Asset>>(assetReference->second.Reference));
				// Create result for loading task
				std::promise<std::tuple<Shared<Asset>, std::exception_ptr>>  promise;
				std::future<std::tuple<Shared<Asset>, std::exception_ptr>>   result(promise.get_future());
				promise.set_value({ assetPtr, nullptr });
				//////////////////////
				AssetLoadingTask task(id, lifeTime, result, callback);
				manager.m_PrefabsTaskList[id] = std::move(task);
			}

		}// We couldn't find so need to load
		else
		{
			manager._LoadNewPrefab<T>(id, callback, lifeTime);
		}

		//return nullptr;
	}

	template<typename T>
	inline void AssetManager::_LoadNewAsset(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime)
	{
		//static_assert(std::is_member_function_pointer<decltype(&T::Create)>::value, "Specified asset doesn't have static Create fucntion");

		auto task = m_AssetsTaskList.find(id);
		if (task != m_AssetsTaskList.end())
		{
			// Task already exist, need reupdate recivers
			task->second.ReceiverCallbacks.push_back(callback);
		}
		else
		{
			auto assetData = m_AssetsDataList.find(id);

			if (assetData != m_AssetsDataList.end())
			{
				AssetLoadingTask task(
					id,
					lifeTime,
					m_TrheadPool.Emplace([=]()
						{
							try
							{
								auto asset = T::Create();
								asset->_SetPrefab(false);
								asset->LoadFromAssetData(assetData->second);
								return std::make_tuple(std::static_pointer_cast<shade::Asset>(asset), std::current_exception());
							}
							catch (...)
							{
								return std::make_tuple(Shared<Asset>(), std::current_exception());
							}

						}),
					callback);

				m_AssetsTaskList[id] = std::move(task);
			}
			else
			{
				SHADE_CORE_WARNING("Specifaed asset id '{0}' hasn't been found.", id)
			}
		}
	}
	template<typename T>
	inline void AssetManager::_LoadNewPrefab(const std::string& id, PostLoadCallback callback, const Asset::Lifetime& lifeTime)
	{
		//static_assert(std::is_member_function_pointer<decltype(&T::Create)>::value, "Specified asset doesn't have static Create fucntion");

		auto task = m_PrefabsTaskList.find(id);
		if (task != m_PrefabsTaskList.end())
		{
			// Task already exist, need reupdate recivers
			task->second.ReceiverCallbacks.push_back(callback);
		}
		else
		{
			auto assetData = m_PrefabDataList.find(id);

			if (assetData != m_PrefabDataList.end())
			{
				AssetLoadingTask task(
					id,
					lifeTime,
					m_TrheadPool.Emplace([=]()
						{
							try
							{
								auto asset = T::Create();
								asset->_SetPrefab(true);
								asset->LoadFromAssetData(assetData->second);
								return std::make_tuple(std::static_pointer_cast<shade::Asset>(asset), std::current_exception());
							}
							catch (...)
							{
								return std::make_tuple(Shared<Asset>(), std::current_exception());
							}

						}),
					callback);

				m_PrefabsTaskList[id] = std::move(task);
			}
			else
			{
				SHADE_CORE_WARNING("Specifaed asset id '{0}' hasn't been found.", id)
			}
		}
	}
	template<typename T>
	inline auto AssetManager::Receive(shade::Shared<shade::Asset>& asset) -> auto
	{
		return std::static_pointer_cast<T>(asset);
	}
}