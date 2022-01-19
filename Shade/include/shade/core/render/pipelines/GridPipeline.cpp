#include "shade_pch.h"
#include "GridPipeline.h"
#include "shade/core/render/Render.h"

shade::GridPipeline::GridPipeline()
{
    m_Shader = ShadersLibrary::Create("Grid", "resources/shaders/Grid.glsl");
}

shade::Shared<shade::FrameBuffer> shade::GridPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	target->Bind();
	m_Shader->Bind();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::GridPipeline::GetResult() const
{
    return nullptr;
}
