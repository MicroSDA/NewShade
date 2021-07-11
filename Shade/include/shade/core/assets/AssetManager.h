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
		// Contain a loading result
		using LoadingResult = std::future<std::tuple<Shared<Asset>, std::exception_ptr>>;
	public:
		friend class Application;
		friend class Asset;
		static bool LoadAssetDataListFromFile(const std::string& filePath);
		// Use for loading new asset into memeory by specific asset id.
		// If you want to kepp an asset in memeory even if all holders was deleted, 
		// use Asset::State::KeepAlive or Asset::State::RemoveIfPosible if you don't want to keep it in memeory.
		// All assets that were held with Asset::State::RemoveIfPosible flag will be removed automatically !
		template<typename T>
		static void Hold(const std::string& id, const Asset::State& state, ReceiverCallback reciverCallback);
		// For casting asset
		template<typename T>
		static auto Receive(shade::Shared<shade::Asset>& asset)->auto;
		// Unload specific asset if ref count is 0
		static void Free(const std::string& id);
		static AssetsDataList& GetAssetsDataList();
	public:
		~AssetManager();
	private:
		struct AssetLoadingTask
		{
			AssetLoadingTask() = default;
			AssetLoadingTask(const std::string& id, const Asset::State& state, LoadingResult& result, ReceiverCallback callback) :
				Id(id), State(state), Result(std::move(result))
			{
				ReceiverCallbacks.push_back(callback);
			}
			std::string						Id;
			Asset::State					State;
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
		void _RecursiveReadingAssetDataList(pugi::xml_node node);
		template<typename T>
		void _LoadNewAsset(const std::string& id, const Asset::State& state, ReceiverCallback reciverCallback);
		void _DispatchAssets();
		// Internal use for asset object only, will remove entry from AssetsDataList
		static void _ImLast(const std::string& assetId);
		// Internal use for Application only
		static void _Clear();

		bool											m_ImDestructing = false;
		std::mutex										m_TasksMutex;
		AssetsDataList									m_AssetsDataList;
		AssetReferences									m_AssetReferences;
		pugi::xml_document								m_AssetsDoc;
		shade::util::ThreadPool							m_TrheadPool;
		std::map<const std::string, AssetLoadingTask>	m_TasksList;
	};
	template<typename T>
	inline void AssetManager::Hold(const std::string& id, const Asset::State& state, ReceiverCallback reciverCallback)
	{
		// Cgeck if tsk exist at first!!!!!!!!!! Issue!!!!!!!
		auto& manager = _Get();
		std::unique_lock<std::mutex> lock{ manager.m_TasksMutex };
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

					auto task = manager.m_TasksList.find(id);

					if (task != manager.m_TasksList.end())
					{
						manager.m_TasksList[id].ReceiverCallbacks.push_back(reciverCallback);
					}
					else
					{
						// Create result for loading task
						std::promise<std::tuple<Shared<Asset>, std::exception_ptr>>  promise;
						std::future<std::tuple<Shared<Asset>, std::exception_ptr>>  result(promise.get_future());
						promise.set_value({ assetPtr, nullptr });
						//////////////////////
						AssetLoadingTask task(id, state, result, reciverCallback);
						manager.m_TasksList[id] = std::move(task);
					}
				}
				else
				{
					// Start to load new asset
					manager._LoadNewAsset<T>(id, state, reciverCallback);
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
				AssetLoadingTask task(id, state, result, reciverCallback);
				manager.m_TasksList[id] = std::move(task);
			}

		}// We couldn't find so need to load
		else
		{
			manager._LoadNewAsset<T>(id, state, reciverCallback);
		}
	}

	template<typename T>
	inline void AssetManager::_LoadNewAsset(const std::string& id, const Asset::State& state, ReceiverCallback reciverCallback)
	{
		//static_assert(std::is_member_function_pointer<decltype(&T::Create)>::value, "Specified asset doesn't have static Create fucntion");

		auto task = m_TasksList.find(id);
		if (task != m_TasksList.end())
		{
			// Task already exist, need reupdate recivers
			task->second.ReceiverCallbacks.push_back(reciverCallback);
		}
		else
		{
			auto assetData = m_AssetsDataList.find(id);
			if (assetData != m_AssetsDataList.end())
			{
				AssetLoadingTask task(
					id,
					state,
					m_TrheadPool.Emplace([=]()
						{
							try
							{
								auto asset = T::Create();
								asset->LoadFromAssetData(assetData->second);
								return std::make_tuple(std::static_pointer_cast<shade::Asset>(asset), std::current_exception());
							}
							catch (...)
							{
								return std::make_tuple(Shared<Asset>(), std::current_exception());
							}

						}),
					reciverCallback);

				m_TasksList[id] = std::move(task);
			}
			else
			{
				SHADE_CORE_WARNING("Specifaed class name '{0}' hasn't been found.", id)
			}
		}
	}
	template<typename T>
	inline auto AssetManager::Receive(shade::Shared<shade::Asset>& asset) -> auto
	{
		return std::static_pointer_cast<T>(asset);
	}
}