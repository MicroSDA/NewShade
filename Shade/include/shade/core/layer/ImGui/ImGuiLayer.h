#pragma once
#include "shade/core/layer/Layer.h"

struct ImGuiViewport;

namespace shade
{
	class SHADE_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name);
		virtual ~ImGuiLayer();
		// Inherited via Layer
		virtual void OnCreate() = 0;
		virtual void OnUpdate(const shade::Shared<Scene>& scene, const shade::Timer& deltaTime) = 0;
		virtual void OnRenderBegin() override;
		virtual void OnRender(const shade::Shared<Scene>& scene) = 0;
		virtual void OnRenderEnd() override;
		virtual void OnDelete() = 0;
	protected:
		int m_WindowFlags;
		int m_DockSpaceFlags;
		ImGuiViewport* m_Viewport;
	};
}


