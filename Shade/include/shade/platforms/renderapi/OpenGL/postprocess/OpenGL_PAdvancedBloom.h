#pragma once
#include "shade/core/render/postprocess/PPAdvancedBloom.h"
#include <glad/glad.h>

namespace shade
{
	class OpenGL_PAdvancedBloom : public PPAdvancedBloom
	{
	public:
		OpenGL_PAdvancedBloom();
		virtual ~OpenGL_PAdvancedBloom() = default;
		// Inherited via PPBloom
		virtual void Process() override;
	private:
	};

}