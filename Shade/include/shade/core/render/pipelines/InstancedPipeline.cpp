#include "shade_pch.h"
#include "InstancedPipeline.h"
#include "shade/core/render/Render.h"
#include "shade/core/render/pipelines/ShadowMapPipeline.h"

shade::InstancedPipeline::InstancedPipeline(const RenderPipeline::Specification& spec)
{
	m_Specification = spec;
}

shade::Shared<shade::FrameBuffer > shade::InstancedPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPiepline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	auto& shader = m_Specification.Shader;
	shader->Bind(); shader->ExecuteSubrutines();

	target->Bind();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			if (material)
			{
				shader->SendFloat3("u_Material.AmbientColor", glm::value_ptr(material->ColorAmbient));
				shader->SendFloat3("u_Material.DiffuseColor", glm::value_ptr(material->ColorDiffuse));
				shader->SendFloat3("u_Material.SpecularColor", glm::value_ptr(material->ColorSpecular));
				shader->SendFloat3("u_Material.TransparentColor", glm::value_ptr(material->ColorTransparent));
				shader->SendFloat("u_Material.Emissive", material->Emmisive);
				shader->SendFloat("u_Material.Shinines", material->Shininess);
				shader->SendFloat("u_Material.ShininesStrength", material->ShininessStrength);
				shader->SendFloat("u_Material.Opacity", material->Opacity);
				shader->SendFloat("u_Material.Refracti", material->Refracti);

				/* Attach texures to their slots*/
				if (material->TextureDiffuse)
					material->TextureDiffuse->Bind(0);
				if (material->TextureSpecular)
					material->TextureSpecular->Bind(1);
				if (material->TextureNormals)
					material->TextureNormals->Bind(2);

				/* If shadows pas*/
				auto* shadowMap = &previusPiepline->As<ShadowMapPipeline>();
				if (shadowMap)
				{
					if (shadowMap->GetDriectLightFrameBuffer())
						shadowMap->GetDriectLightFrameBuffer()->BindDepthAsTexture(3);
					if (shadowMap->GetSpotLightFrameBuffer())
						shadowMap->GetSpotLightFrameBuffer()->BindDepthAsTexture(4);
				}
			}

			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	shader->UnBind();

	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::InstancedPipeline::GetResult() const
{
	return nullptr;
}

