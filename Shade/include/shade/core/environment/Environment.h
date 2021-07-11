#pragma once
#include "shade/config/API.h"
#include "shade/core/render/Shader.h"

namespace shade
{
	class SHADE_API Environment
	{
	public:
		enum class Type : std::uint32_t
		{
			Environment,
			DirectLight,
			PointLight,
			SpotLight,
			Fog
		};
		Environment(const Environment::Type& type);
		virtual ~Environment() = default;
		const Environment::Type& GetType() const;
		virtual void Process(const Shared<Shader>& shader) = 0;
	protected:
		const Environment::Type m_Type;
	private:
	};
}