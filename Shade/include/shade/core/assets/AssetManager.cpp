#include "shade_pch.h"
#include "AssetManager.h"

shade::AssetManager::AssetManager() :
	m_TrheadPool(std::thread::hardware_concurrency() - 1) // Create 3 threads
{
}

shade::AssetManager::~AssetManager()
{
	// Trying to evade recursive deleting
	_Get().m_ImDestructing = true;
}

bool shade::AssetManager::LoadAssetDataListFromFile(const std::string& filePath)
{
	if (filePath.empty())
	{
		SHADE_CORE_ERROR("Path of Asset data list is empty !");
		return false;
	}
	else
	{
		auto& instance = _Get();
		instance.m_AssetsDoc.load_file(filePath.c_str());
		if (instance.m_AssetsDoc.child("assets").empty())
		{
			SHADE_CORE_ERROR("Assets were not been found in '{0}'", filePath);
			return false;
		}
		instance._ReadAssetDataList(instance.m_AssetsDoc.child("assets"));
		instance._RecursiveReadingPrefabsDataList(instance.m_AssetsDoc.child("assets"));
		return true;
	}
}

void shade::AssetManager::Free(const std::string& id)
{
	auto& instance = _Get();
	auto reference = instance.m_AssetReferences.find(id);

	if (reference != instance.m_AssetReferences.end())
	{

		if (std::get_if<shade::Shared<shade::Asset>>(&reference->second.Reference))
		{
			if (std::get<shade::Shared<shade::Asset>>(reference->second.Reference).use_count() == 1)
			{
				instance.m_AssetReferences.erase(reference);
			}
			else
			{
				SHADE_CORE_WARNING("Trying to free asset '{0}' which has more then one uses count.", id)
			}

		}
		else
		{
			SHADE_CORE_WARNING("Trying to free weak asset '{0}'.", id)
		}
	}
	else
	{
		SHADE_CORE_WARNING("Trying to free unloaded asset '{0}'.", id)
	}
}

shade::AssetManager::AssetsDataList& shade::AssetManager::GetAssetsDataList()
{
	return _Get().m_AssetsDataList;
}

shade::AssetData shade::AssetManager::GetAssetData(const std::string& id)
{
	auto& instance = _Get();

	auto assetData = instance.m_AssetsDataList.find(id);
	if (assetData != instance.m_AssetsDataList.end())
		return AssetData(assetData->second);
	else
	{
		assetData = instance.m_PrefabDataList.find(id);
		if(assetData == instance.m_PrefabDataList.end())
			SHADE_CORE_WARNING("Couldn't find specified asset data by asset id : {0}", id)
		else
			return AssetData();
	}
}

shade::AssetManager::AssetsDataList& shade::AssetManager::GetPrefabsDataList()
{
	return _Get().m_PrefabDataList;
}

shade::AssetData shade::AssetManager::GetPrefabsData(const std::string& id)
{
	auto& instance = _Get();

	auto assetData = instance.m_PrefabDataList.find(id);
	if (assetData != instance.m_PrefabDataList.end())
		return AssetData(assetData->second);
	else
	{
		SHADE_CORE_WARNING("Couldn't find specified prefab data by asset id :{0}", id);
		return AssetData();
	}
}

shade::AssetManager& shade::AssetManager::_Get()
{
	static AssetManager instatnce;
	return instatnce;
}

void shade::AssetManager::_RecursiveReadingPrefabsDataList(pugi::xml_node node)
{
	if (!node.child("prefab").empty())
	{
		for (auto asset : node.children("prefab"))
		{
			std::string id = asset.attribute("Id").as_string();

			if (m_PrefabDataList.find(id) != m_PrefabDataList.end())
			{
				SHADE_CORE_WARNING("Current asset id already exists '{0}'. Skipped...", id);
				continue;
			}
			else
			{
				m_PrefabDataList.emplace(id, AssetData(asset));
				for (auto& dependency : asset.children("dependencies"))
				{
					_RecursiveReadingPrefabsDataList(dependency);
				}
			}
		}
	}
}

void shade::AssetManager::_ReadAssetDataList(pugi::xml_node node)
{
	if (!node.child("asset").empty())
	{
		for (auto& asset : node.children("asset"))
		{
			std::string id = asset.attribute("Id").as_string();

			if (m_AssetsDataList.find(id) != m_AssetsDataList.end())
			{
				SHADE_CORE_WARNING("Current asset id already exists '{0}'. Skipped...", id);
				continue;
			}
			else
			{
				m_AssetsDataList.emplace(id, AssetData(asset));
			}
		}
	}

}

void shade::AssetManager::_DispatchAssets(std::map<const std::string, AssetLoadingTask>& taskList, shade::AssetManager::AssetReferences& references, std::mutex& mutex)
{
	std::unique_lock<std::mutex> lock{ mutex, std::defer_lock };

	if (lock.try_lock())
	{
		if (taskList.empty())
			return;

		auto task = taskList.begin();
		while (task != taskList.end())
		{
			if (task->second.Result._Is_ready())
			{
				try {

					auto result = task->second.Result.get();

					if (std::get<1>(result) != nullptr)
						std::rethrow_exception(std::get<1>(result));

					if (task->second.State == shade::Asset::Lifetime::KeepAlive)
					{
						// Should be saved as shared
						references.emplace(std::pair<std::string, AssetReference>(task->second.Id, AssetReference{ shade::Shared<shade::Asset>(std::get<0>(result)) }));
					}
					else
					{   // Should be saved as weak
						references.emplace(std::pair<std::string, AssetReference>(task->second.Id, AssetReference{ shade::Weak<shade::Asset>(std::get<0>(result)) }));
					}

					for (auto& callback : task->second.ReceiverCallbacks)
					{
						callback(std::get<0>(result));
					}

					std::get<0>(result)->AssetInit();
					task = taskList.erase(task);
				}
				catch (std::exception& exc)
				{
					SHADE_CORE_ERROR("Asset load exception: {0}", exc.what());
					task = taskList.erase(task);
				}
			}
			else
			{
				++task;
			}
		}
	}
}

void shade::AssetManager::_Dispatch()
{
	auto& instance = _Get();

	/* Dispatch prefabs */
	_DispatchAssets(instance.m_PrefabsTaskList, instance.m_PrefabReferences, instance.m_PrefabsTaskMutex);
	/* Dispatch assets */
	_DispatchAssets(instance.m_AssetsTaskList, instance.m_AssetReferences,  instance.m_AssetsTaskMutex);
}

void shade::AssetManager::_ImLast(const std::string& assetId, const bool& isPrefab)
{
	auto& instance = _Get();
	if (!instance.m_ImDestructing)
	{
		if (isPrefab)
		{
			auto reference = instance.m_PrefabReferences.find(assetId);
			if (reference != instance.m_PrefabReferences.end())
			{
				instance.m_PrefabReferences.erase(reference);
			}
		}
		else
		{
			auto reference = instance.m_AssetReferences.find(assetId);
			if (reference != instance.m_AssetReferences.end())
			{
				instance.m_AssetReferences.erase(reference);
			}
		}
	}
}

void shade::AssetManager::_Clear()
{
	// Trying to evade recursive deleting
	_Get().m_ImDestructing = true;
	_Get().m_AssetReferences.clear();
}
