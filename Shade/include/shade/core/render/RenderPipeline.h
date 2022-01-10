#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/FrameBuffer.h"
//#include "shade/core/render/Render.h"
#include "shade/core/render/Shader.h"
#include "shade/core/render/drawable/Drawable.h"
#include "shade/core/render/DrawablePass.h"

#include <glm/glm.hpp>

namespace shade
{
	class SHADE_API RenderPipeline
	{
	public:
		struct Specification
		{
			Shared<Shader>					Shader;
		};
	public:
		template<typename T>
		static Shared<T> Create(const Specification& spec);
		template<typename T>
		T& As();
		virtual ~RenderPipeline() = default;
		virtual const Shared<FrameBuffer>& GetResult()  const = 0;
		const Specification& GetSpecification() const;
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) = 0;
	protected:
		Specification		m_Specification;
	};
	template<typename T>
	inline Shared<T> RenderPipeline::Create(const RenderPipeline::Specification& spec)
	{
		return shade::CreateShared<T>(spec);
	}
	template<typename T>
	inline T& RenderPipeline::As()
	{
		static_assert(std::is_base_of<RenderPipeline, T>::value, "");
		return static_cast<T&>(*this);
	}
	
}