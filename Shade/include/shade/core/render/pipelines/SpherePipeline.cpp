#include "shade_pch.h"
#include "SpherePipeline.h"
#include "shade/core/render/Render.h"

shade::SpherePipeline::SpherePipeline()
{
    m_Shader = ShadersLibrary::Create("Sphere", "resources/shaders/Util/Sphere.glsl");
}

shade::SpherePipeline::~SpherePipeline()
{
}

shade::Shared<shade::FrameBuffer> shade::SpherePipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPipline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_Shader->Bind();
	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			if (drawData[instance].TBO->GetSize() != sizeof(glm::mat4) * transforms.size())
				drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::SpherePipeline::GetResult() const
{
    return nullptr;
}
