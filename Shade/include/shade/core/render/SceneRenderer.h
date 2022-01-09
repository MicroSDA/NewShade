#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/render/drawable/Drawable.h"
#include "shade/core/mesh/Material3D.h"
#include "shade/core/render/DrawablePass.h"


namespace shade
{
	
	class SHADE_API SceneRenderer
	{
	public:
		SceneRenderer();
		static Shared<SceneRenderer> Create();
		virtual ~SceneRenderer() = default;

		Shared<FrameBuffer> ShadowMappingPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
		Shared<FrameBuffer> GeneralPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
		Shared<FrameBuffer> BloomPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
		/* Offset */
		Shared<FrameBuffer> GridPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
		Shared<FrameBuffer> AABBPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
		Shared<FrameBuffer> CameraFrustumPass(const shade::Shared<FrameBuffer>& target, const shade::Shared<FrameBuffer>& previousPass, const DrawablePools& drawables, std::unordered_map<Shared<Drawable>, BufferDrawData>& drawData);
	private:
		/* Shaders */
		Shared<Shader> m_GeneralPassShader;
		Shared<Shader> m_ShadowMappingPassShader;
		Shared<Shader> m_GridPassShader;
		Shared<Shader> m_AABBPassShader;
		Shared<Shader> m_BloomShader;
		Shared<Shader> m_CameraFrustumShader;

		/* FrameBuffers */

		Shared<FrameBuffer> m_DirectLightShadowFrameBuffer;
	};

}