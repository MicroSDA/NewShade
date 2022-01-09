#include "shade_pch.h"
#include "SceneRenderer.h"
#include "shade/core/render/Render.h"

shade::SceneRenderer::SceneRenderer()
{
	/* TODO create frame buffers and soon! */
	m_GeneralPassShader       = ShadersLibrary::Get("General");
	m_ShadowMappingPassShader = ShadersLibrary::Get("DirectLightShadow");
	m_GridPassShader          = ShadersLibrary::Get("Grid");

	m_DirectLightShadowFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(4096, 4096, {
		shade::FrameBuffer::Texture::Format::DEPTH24STENCIL8_ARRAY_4 }));
}

shade::Shared<shade::SceneRenderer> shade::SceneRenderer::Create()
{
	return CreateShared<SceneRenderer>();
}

shade::Shared<shade::FrameBuffer> shade::SceneRenderer::GeneralPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_GeneralPassShader;
	m_GeneralPassShader->Bind(); m_GeneralPassShader->ExecuteSubrutines();

	target->Bind();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			if (material)
			{
				m_GeneralPassShader->SendFloat3("u_Material.AmbientColor", glm::value_ptr(material->ColorAmbient));
				m_GeneralPassShader->SendFloat3("u_Material.DiffuseColor", glm::value_ptr(material->ColorDiffuse));
				m_GeneralPassShader->SendFloat3("u_Material.SpecularColor", glm::value_ptr(material->ColorSpecular));
				m_GeneralPassShader->SendFloat3("u_Material.TransparentColor", glm::value_ptr(material->ColorTransparent));
				m_GeneralPassShader->SendFloat("u_Material.Emissive", material->Emmisive);
				m_GeneralPassShader->SendFloat("u_Material.Shinines", material->Shininess);
				m_GeneralPassShader->SendFloat("u_Material.ShininesStrength", material->ShininessStrength);
				m_GeneralPassShader->SendFloat("u_Material.Opacity", material->Opacity);
				m_GeneralPassShader->SendFloat("u_Material.Refracti", material->Refracti);

				/* Attach texures to their slots*/
				if (material->TextureDiffuse)
					material->TextureDiffuse->Bind(0);
				if (material->TextureSpecular)
					material->TextureSpecular->Bind(1);
				if (material->TextureNormals)
					material->TextureNormals->Bind(2);

				/* If shadows pas*/
				if (previousPass)
					previousPass->BindDepthAsTexture(3);
			}

			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	m_GeneralPassShader->UnBind();

	return target;
}

shade::Shared<shade::FrameBuffer> shade::SceneRenderer::ShadowMappingPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_ShadowMappingPassShader;
	m_ShadowMappingPassShader->Bind(); m_ShadowMappingPassShader->ExecuteSubrutines();

	m_DirectLightShadowFrameBuffer->Clear(AttacmentClearFlag::Depth);
	/*  Before you render objects to the CSM, call glEnable(GL_DEPTH_CLAMP); and after, call glDisable(GL_DEPTH_CLAMP);
		This will cause every object that gets rendered to not get cut out, so artifacts where some cascades cut out some geometry will disappear. */
	Render::GetRenderApi()->Enable(0x864F); // DEPTH_CLAMP
	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(Drawable::DrawMode::Triangles, drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	m_ShadowMappingPassShader->UnBind();
	Render::GetRenderApi()->Disable(0x864F);

	return m_DirectLightShadowFrameBuffer;
}
shade::Shared<shade::FrameBuffer> shade::SceneRenderer::BloomPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	
	m_BloomShader;

	/* Has to be revorked, no need actual pass, just a blum pp exec !*/

	return target;
}

shade::Shared<shade::FrameBuffer> shade::SceneRenderer::GridPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_GridPassShader;
	m_GridPassShader->Bind(); m_GridPassShader->ExecuteSubrutines();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	m_GridPassShader->UnBind();

	return target;
}

shade::Shared<shade::FrameBuffer> shade::SceneRenderer::AABBPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	m_AABBPassShader;
	m_AABBPassShader->Bind(); m_AABBPassShader->ExecuteSubrutines();

	for (auto& [instance, materials] : drawables.Drawables)
	{
		for (auto& [material, transforms] : materials.Materials)
		{
			drawData[instance].TBO->Resize(sizeof(glm::mat4) * transforms.size());
			drawData[instance].TBO->SetData(transforms.data(), sizeof(glm::mat4) * transforms.size(), 0);

			Render::GetRenderApi()->DrawInstanced(instance->GetDrawMode(), drawData[instance].VAO, drawData[instance].IBO, transforms.size());
		}
	}
	m_AABBPassShader->UnBind();

	return target;
}

shade::Shared<shade::FrameBuffer> shade::SceneRenderer::CameraFrustumPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData)
{
	// TODO !
	return target;
}
