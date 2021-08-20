#include "shade_pch.h"
#include "Scene.h"
#include <yaml-cpp/yaml.h>
#include "shade/core/scene/SceneSerrializing.h"

shade::Shared<shade::Scene> shade::Scene::Create()
{
	return CreateShared<Scene>();
}

shade::Scene::~Scene()
{
	DestroyEntities();
}

void shade::Scene::NativeScriptsUpdate(const shade::Timer& deltaTime)
{
	this->GetEntities().view<NativeScriptComponent>().each([=](auto entity, auto& script)
		{
			if (!script.m_pInstance)
			{
				script.m_pInstance = script.InstantiateScript();
				script.m_pInstance->m_Entity = shade::Entity{ entity, this };
				script.m_pInstance->OnCreate();
			}
			else if (script.m_pInstance->IsUpdate())
				script.m_pInstance->OnUpdate(deltaTime);
		});
}

const std::string& shade::Scene::GetName() const
{
	return m_Name;
}

shade::Entity shade::Scene::GetPrimaryCamera()
{
	this->GetEntities().view<shade::CameraComponent>().each([&](auto entity, auto& camera)
		{
			if(camera->IsPrimary())
				return Entity{ entity, this };
		});

	return Entity(); // Make use that entity is valid !
}

bool shade::Scene::Serialize(std::ostream& stream) const
{
	return false;
}

bool shade::Scene::Deserialize(std::istream& stream)
{
	return false;
}

bool shade::Scene::Serialize() const
{
	if (!GetAssetData().Serialize())
		return false;

	SHADE_CORE_TRACE("Star of scene serializing !");

	std::string id = GetAssetData().Attribute("Id").as_string();
	std::string path = GetAssetData().Attribute("Path").as_string();
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << id;
	out << YAML::Key << "Entities" << YAML::BeginSeq;

	GetEntities().each([&](auto entity)
		{
			Entity _entity{ entity, const_cast<Scene*>(this) };

			if (_entity.IsValid())
			{
				if (!SerrEntity(_entity, out))
				{
					SHADE_CORE_WARNING("Entity id: '{0}' has not been serialized successful!", entity);
				}
				else
				{
					SHADE_CORE_TRACE("Entity id: '{0}' serialized successful.", entity);
				}
			}
		});

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream file;
	file.open(path);
	if (file.is_open())
	{
		file << out.c_str();
		file.close();
		SHADE_CORE_TRACE("Scene '{0}' has been serialized successful.", id)
			return true;
	}
	else
	{
		SHADE_CORE_WARNING("Scene '{0}' has not been serialized successful.", id)
			return false;
	}
}

bool shade::Scene::Deserialize()
{
	std::string path	= GetAssetData().Attribute("Path").as_string();
	std::string id		= GetAssetData().Attribute("Id").as_string();

	if (std::filesystem::exists(path))
	{
		YAML::Node scene = YAML::LoadFile(path);

		auto entities = scene["Entities"];
		if (entities)
		{
			for (auto& entity : entities)
			{
				auto _entity = CreateEntity(entity["Tag"].as<shade::Tag>());
				auto entityId = entity["Entity"].as<std::uint32_t>();
				if (!DeserrEntity(_entity, entity))
				{
					SHADE_CORE_WARNING("Entity id: {0} has not been deserialized successful.", entityId);
				}
				else
				{
					SHADE_CORE_TRACE("Entity id: {0} deserialization has been done.", entityId);
				}
			}
		}

		return true;
	}
	else
	{
		SHADE_CORE_WARNING("Scene id: {0} doesn't exist", id);
		return false;
	}
	
}

void shade::Scene::LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle)
{
	SetAssetData(data);
	Deserialize();
}

void shade::Scene::AssetInit()
{
}
