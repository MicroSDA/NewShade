#pragma once
#include "shade/config/API.h"
#include "shade/core/serializing/Serializable.h"
#include "shade/core/utils/Log.h"
#include <pugixml/pugixml.hpp>

namespace shade
{
	class SHADE_API AssetData : public Serializable
	{
	public:
		AssetData();
		AssetData(pugi::xml_node data);
		~AssetData() = default;
		AssetData(const AssetData& other):
			m_IsValid(false)
		{
			if (this != &other)
			{
				this->m_IsValid = other.m_IsValid;
				this->m_AssetData = other.m_AssetData;
				this->m_Root.append_copy(m_Root);
			}
		}
		AssetData& operator=(const AssetData& other)
		{
			if (this != &other)
			{
				this->m_IsValid = other.m_IsValid;
				this->m_AssetData = other.m_AssetData;
				this->m_Root.append_copy(m_Root);
			}
			return *this;
		}
	public:
		void					Append(AssetData& data);
		const bool&				IsValid() const;
		AssetData				Parent();
		std::vector<AssetData>	Dependencies() const;

		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	public:
		pugi::xml_attribute		Attribute(const std::string& attr) const;
		pugi::xml_node			_Raw() const;
	private:
		mutable pugi::xml_document		m_Root;
		pugi::xml_node					m_AssetData;
		bool							m_IsValid;
	};
}