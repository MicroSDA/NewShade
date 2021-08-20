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
		SHADE_CORE_WARNING("Couldn't find specified asset data by asset id :{0}", id);
		return AssetData();
	}
}

shade::AssetManager& shade::AssetManager::_Get()
{
	static AssetManager instatnce;
	return instatnce;
}

void shade::AssetManager::_RecursiveReadingAssetDataList(pugi::xml_node node)
{
	if (!node.child("asset").empty())
	{
		for (auto asset : node.children("asset"))
		{
			std::string id = asset.attribute("id").as_string();

			if (m_AssetsDataList.find(id) != m_AssetsDataList.end())
			{
				SHADE_CORE_WARNING("Current asset id already exists '{0}'. Skipped...", id);
				continue;
			}
			else
			{
				m_AssetsDataList.emplace(id, AssetData(asset));
				for (auto& dependency : asset.children("dependencies"))
				{
					_RecursiveReadingAssetDataList(dependency);
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


	/*pugi::xml_document doc;
	auto n = doc.append_child("assets");
	for (auto asset : m_AssetsDataList)
	{
		n.append_copy(asset.second._Raw());
	}

	doc.save_file("Test.xml");*/
}

void shade::AssetManager::_DispatchAssets()
{
	auto& instance = _Get();
	std::unique_lock<std::mutex> lock{ instance.m_TasksMutex, std::defer_lock };

	if (lock.try_lock())
	{
		if (instance.m_TasksList.empty())
			return;

		auto task = instance.m_TasksList.begin();
		while (task != instance.m_TasksList.end())
		{
			if (task->second.Result._Is_ready())
			{
				try {

					auto result = task->second.Result.get();

					if (std::get<1>(result) != nullptr)
						std::rethrow_exception(std::get<1>(result));

					if (task->second.State == shade::Asset::State::KeepAlive)
					{
						// Should be saved as shared
						instance.m_AssetReferences.emplace(std::pair<std::string, AssetReference>(task->second.Id, AssetReference{ shade::Shared<shade::Asset>(std::get<0>(result)) }));
					}
					else
					{   // Should be saved as weak
						instance.m_AssetReferences.emplace(std::pair<std::string, AssetReference>(task->second.Id, AssetReference{ shade::Weak<shade::Asset>(std::get<0>(result)) }));
					}

					for (auto& callback : task->second.ReceiverCallbacks)
					{
						callback(std::get<0>(result));
					}
					
					std::get<0>(result)->AssetInit();
					task = instance.m_TasksList.erase(task);
				}
				catch (std::exception& exc)
				{
					SHADE_CORE_ERROR("Asset load exception: {0}", exc.what());
					task = instance.m_TasksList.erase(task);
				}
			}
			else
			{
				++task;
			}
		}
	}
}

void shade::AssetManager::_ImLast(const std::string& assetId)
{
	auto& instance = _Get();
	if (!instance.m_ImDestructing)
	{
		auto reference = instance.m_AssetReferences.find(assetId);
		if (reference != instance.m_AssetReferences.end())
		{
			instance.m_AssetReferences.erase(reference);
		}
	}
}

void shade::AssetManager::_Clear()
{
	// Trying to evade recursive deleting
	_Get().m_ImDestructing = true;
	_Get().m_AssetReferences.clear();
}

std::tuple<std::string, std::string> shade::AssetManager::_IsBundle(const std::string& bundle)
{
	const char delim = '/';
	size_t start = 0;
	size_t end = bundle.find(delim);
	std::vector<std::string> _bundle;
	std::tuple<std::string, std::string> out;

	if (end != std::string::npos)
	{
		while (end != std::string::npos)
		{
			_bundle.push_back(bundle.substr(start, end - start));
			//std::cout << bundle.substr(start, end - start) << std::endl;
			start = end + 1; // delim size
			end = bundle.find(delim, start);
		}

		_bundle.push_back(bundle.substr(start, end - start));
 		//std::cout << bundle.substr(start, end) << std::endl;
	}
	else
	{
		return { "",bundle };// single one
		//_bundle.push_back(bundle); // single one
	}
	
	return {"Cube/Mesh" ,_bundle.back()};

	//if(start != std::string::npos && end != std::string::npos)
	
}
