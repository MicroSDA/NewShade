#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"

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