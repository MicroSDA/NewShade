#pragma once
#include "shade/config/API.h"
#include "shade/core/utils/Binarizer.h"

namespace shade
{
	class SHADE_API Serializable
	{
	public:
		Serializable() = default;
		virtual ~Serializable() = default;

		virtual bool Serialize(std::ostream& stream) const = 0;
		virtual bool Deserialize(std::istream& stream) = 0;
		virtual bool Serialize() const = 0;
		virtual bool Deserialize() = 0;
	};
}