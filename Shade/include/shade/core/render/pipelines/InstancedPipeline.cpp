#include "shade_pch.h"
#include "InstancedPipeline.h"
#include "shade/core/render/Render.h"
#include "shade/core/render/pipelines/ShadowMapPipeline.h"

shade::InstancedPipeline::InstancedPipeline()
{
	m_Shader = ShadersLibrary::Create("General", "resources/shaders/General/General.glsl");
	m_Shader->SelectSubrutine("u_sLighting", "BillinPhong", shade::Shader::Type::Fragment);
}

shade::Shared<shade::FrameBuffer > shade::InstancedPipeline::Process(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const Shared<RenderPipeline>& previusPiepline, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	target->Bind();
	m_Shader->Bind(); m_Shader->ExecuteSubrutines();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			if (material)
			{
				m_Shader->SendFloat3("u_Material.AmbientColor", glm::value_ptr(material->ColorAmbient));
				m_Shader->SendFloat3("u_Material.DiffuseColor", glm::value_ptr(material->ColorDiffuse));
				m_Shader->SendFloat3("u_Material.SpecularColor", glm::value_ptr(material->ColorSpecular));
				m_Shader->SendFloat3("u_Material.TransparentColor", glm::value_ptr(material->ColorTransparent));
				m_Shader->SendFloat("u_Material.Emissive", material->Emmisive);
				m_Shader->SendFloat("u_Material.Shinines", material->Shininess);
				m_Shader->SendFloat("u_Material.ShininesStrength", material->ShininessStrength);
				m_Shader->SendFloat("u_Material.Opacity", material->Opacity);
				m_Shader->SendFloat("u_Material.Refracti", material->Refracti);

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
					if (shadowMap->GetPointLightFrameBuffer())
						shadowMap->GetPointLightFrameBuffer()->BindDepthAsTexture(5);
				}
			}

			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	m_Shader->UnBind();

	return target;
}

const shade::Shared<shade::FrameBuffer>& shade::InstancedPipeline::GetResult() const
{
	return nullptr;
}

