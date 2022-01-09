#include "shade_pch.h"
#include "GridPipeline.h"
#include "shade/core/render/Render.h"

shade::GridPipeline::GridPipeline(const RenderPipeline::Specification& spec)
{
    m_Specification = spec;
}

shade::Shared<shade::FrameBuffer> shade::GridPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	auto& shader = m_Specification.Shader;
	shader->Bind(); //shader->ExecuteSubrutines();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}

	shader->UnBind();
	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::GridPipeline::GetResult() const
{
    return nullptr;
}
