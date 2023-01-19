#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/utils/Log.h"


namespace shade
{
	class SHADE_API RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Unique<RenderContext> Create(void* window);
	};
}