#pragma once
#include "shade/core/render/postprocess/PPBloom.h"
#include <glad/glad.h>
namespace shade
{
	class OpenGL_PBloom : public PPBloom
	{
	public:
		OpenGL_PBloom();
		virtual ~OpenGL_PBloom() = default;
		// Inherited via PPBloom
		virtual void Process() override;
	private:
	};

}