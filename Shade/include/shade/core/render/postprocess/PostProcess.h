#pragma once
#include "shade/config/API.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/Shader.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shade
{
	class Render;

	class SHADE_API PostProcess
	{
	public:
		friend class Render;
		virtual ~PostProcess() = default;
	protected:
		
		virtual void Process() = 0;
	};
}