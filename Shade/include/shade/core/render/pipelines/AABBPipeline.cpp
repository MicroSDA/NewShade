#include "shade_pch.h"
#include "AABBPipeline.h"
#include "shade/core/render/Render.h"

shade::AABBPipeline::AABBPipeline()
{
	m_Shader = ShadersLibrary::Create("AABB", "resources/shaders/Util/AABB.glsl");
}

shade::Shared<shade::FrameBuffer> shade::AABBPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_Shader->Bind();
	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			if(drawData[instance].TBO->GetSize() != sizeof(glm::mat4) * transforms.size())
				drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Lines, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::AABBPipeline::GetResult() const
{
	return nullptr;
}
