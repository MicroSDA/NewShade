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
		template<typename T>
		T& As();
	protected:
		const Environment::Type m_Type;
	private:
	};
	template<typename T>
	inline T& Environment::As()
	{
		static_assert(std::is_base_of<Environment, T>::value, "");
		return static_cast<T&>(*this); //  Upcast
	}
	//////////////////////
	struct LightEnviroment
	{
		Environment*			Sources;
		//Transform3DComponent*	Transforms;
		std::size_t				Count;
	};
}