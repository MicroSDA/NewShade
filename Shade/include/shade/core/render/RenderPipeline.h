#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/FrameBuffer.h"
//#include "shade/core/render/Render.h"
#include "shade/core/render/Shader.h"
#include "shade/core/render/drawable/Drawable.h"
#include "shade/core/render/DrawablePass.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include <glm/glm.hpp>

namespace shade
{
	class SHADE_API RenderPipeline
	{
	public:
		template<typename T>
		static Shared<T> Create();
		template<typename T>
		T& As();
		virtual ~RenderPipeline() = default;
		virtual const Shared<FrameBuffer>& GetResult()  const = 0;
		virtual Shared<FrameBuffer> Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData) = 0;
	};
	template<typename T>
	inline Shared<T> RenderPipeline::Create()
	{
		return shade::CreateShared<T>();
	}
	template<typename T>
	inline T& RenderPipeline::As()
	{
		static_assert(std::is_base_of<RenderPipeline, T>::value, "");
		return static_cast<T&>(*this);
	}
	
}