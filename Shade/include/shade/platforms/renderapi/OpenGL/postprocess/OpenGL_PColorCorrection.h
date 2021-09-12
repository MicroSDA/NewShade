#pragma once
#include "shade/core/render/postprocess/PPColorCorrection.h"
#include <glad/glad.h>

namespace shade
{
	class OpenGL_PColorCorrection : public PPColorCorrection
	{
	public:
		OpenGL_PColorCorrection();
		virtual ~OpenGL_PColorCorrection() = default;
		// Inherited via PPBloom
		virtual void Process() override;
	private:
	};

}