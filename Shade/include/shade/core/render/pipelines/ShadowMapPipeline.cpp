#include "shade_pch.h"
#include "ShadowMapPipeline.h"
#include "shade/core/render/Render.h"

shade::ShadowMapPipeline::ShadowMapPipeline(const RenderPipeline::Specification& spec)
{
	m_Specification = spec;

	m_DirectLightShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(4096, 4096, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8_ARRAY_4 }));
}

shade::Shared<shade::FrameBuffer> shade::ShadowMapPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	auto& shader = m_Specification.Shader;
	shader->Bind(); shader->ExecuteSubrutines();

	m_DirectLightShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
	/*  Before you render objects to the CSM, call glEnable(GL_DEPTH_CLAMP); and after, call glDisable(GL_DEPTH_CLAMP);
		This will cause every object that gets rendered to not get cut out, so artifacts where some cascades cut out some geometry will disappear. */
	Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
	for (auto &[instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	Render::GetRenderApi()->Disable(0x864F);
	shader->UnBind();

	return m_DirectLightShadowFrameBuffer;
}

const shade::Shared<shade::FrameBuffer>& shade::ShadowMapPipeline::GetResult() const
{
	return m_DirectLightShadowFrameBuffer;
}
