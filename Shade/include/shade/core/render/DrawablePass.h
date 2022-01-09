#pragma once
#include "shade/core/mesh/Material3D.h"
#include "shade/core/render/drawable/Drawable.h"

namespace shade
{
	struct MaterialPools
	{
		std::unordered_map<Shared<Material3D>, std::vector<glm::mat4>>	Materials;
	};

	struct DrawablePools
	{
		std::unordered_map<Shared<Drawable>, MaterialPools> Drawables;
	};

	struct BufferDrawData
	{
		Shared<VertexArray>  VAO;
		Shared<VertexBuffer> VBO;
		Shared<VertexBuffer> TBO;
		Shared<IndexBuffer>  IBO;
	};
}