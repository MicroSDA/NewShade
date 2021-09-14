#include "shade_pch.h"
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "shade/core/entities/Entity.h"
#include "shade/core/components/Components.h"
#include "shade/core/assets/AssetManager.h"

namespace YAML {
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

static bool SerrTagComponent(shade::Entity& entity, YAML::Emitter& emitter)
{
	if (entity.HasComponent<shade::Tag>())
	{
		auto& comp = entity.GetComponent<shade::Tag>();
		emitter << YAML::Key << "Tag" << YAML::Value << comp;
	}

	return true;
}

static bool SerrTransform3DComponent(shade::Entity& entity, YAML::Emitter& emitter)
{
	if (entity.HasComponent<shade::Transform3DComponent>())
	{
		auto& comp = entity.GetComponent<shade::Transform3DComponent>();
		emitter << YAML::Key << "Transform3D" << YAML::BeginMap;
		emitter << YAML::Key << "Position" << YAML::Value << comp.GetPosition();
		emitter << YAML::Key << "Rotation" << YAML::Value << comp.GetRotation();
		emitter << YAML::Key << "Scale" << YAML::Value << comp.GetScale();
		emitter << YAML::EndMap;
	}

	return true;
}

static bool SerrModel3DComponent(shade::Entity& entity, YAML::Emitter& emitter)
{
	if (entity.HasComponent<shade::Model3DComponent>())
	{
		auto& component = entity.GetComponent<shade::Model3DComponent>();
		emitter << YAML::Key << "Model3D" << YAML::BeginMap;
		emitter << YAML::Key << "Asset" << YAML::Value << component->GetAssetData().Attribute("Id").as_string();
		emitter << YAML::EndMap;
	}

	return true;
}

static bool SerrEntity(shade::Entity& entity, YAML::Emitter& emitter)
{
	emitter << YAML::BeginMap << YAML::Key << "Entity" << YAML::Value << static_cast<std::uint32_t>(entity);
	if (!SerrTagComponent(entity, emitter))
		return false;
	if (!SerrTransform3DComponent(entity, emitter))
		return false;
	if (!SerrModel3DComponent(entity, emitter))
		return false;


	emitter << YAML::EndMap;

	return true;
}

//static bool DeserrTag(shade::Entity& entity, YAML::detail::iterator_value& value)
//{
//	if (value["Tag"])
//	{
//		entity.GetComponent<>
//	}
//}

static bool DeserrTransform3DComponent(shade::Entity& entity, YAML::detail::iterator_value& value)
{
	if (value["Transform3D"])
	{
		auto transform = value["Transform3D"];
		auto& _transform = entity.AddComponent<shade::Transform3DComponent>();

		_transform.SetPostition(transform["Position"].as<glm::vec3>());
		_transform.SetRotation(transform["Rotation"].as<glm::vec3>());
		_transform.SetScale(transform["Scale"].as<glm::vec3>());
	}

	return true;
}
static bool DesserModel3DComponent(shade::Entity& entity, YAML::detail::iterator_value& value)
{
	if (value["Model3D"])
	{
		shade::AssetManager::HoldPrefab<shade::Model3D>(value["Model3D"]["Asset"].as<std::string>(),
		[entity](auto& asset) mutable
			{
				entity.AddComponent<shade::Model3DComponent>(shade::AssetManager::Receive<shade::Model3D>(asset));
			}, shade::Asset::Lifetime::Destroy);
	}

	return true;
}
static bool DeserrEntity(shade::Entity& entity, YAML::detail::iterator_value& value)
{
	DeserrTransform3DComponent(entity, value);
	DesserModel3DComponent(entity, value);
	return true;
}