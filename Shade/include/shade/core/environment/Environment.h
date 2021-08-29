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
		void* ReciveRenderData();
	protected:
		const Environment::Type m_Type;
	private:
	};
	template<typename T>
	inline void* Environment::ReciveRenderData()
	{
		return (void*)&static_cast<T*>(this)->GetRenderData(); //  Upcast
	}
}