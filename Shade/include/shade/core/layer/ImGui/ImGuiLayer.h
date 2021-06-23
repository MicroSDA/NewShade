#pragma once
#include "shade/core/layer/Layer.h"
#include <ImGui/imgui.h>

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

	protected:
		// Support functions
		template<typename Callback, typename ...Args>
		void ShowWindowBar(const char* title, Callback callback, Args&& ...args);
		template<typename Comp, typename Call, typename ...Args>
		void DrawComponent(const char* title, Entity& entity, Call callback, Args&& ...args);
		template<typename Comp>
		bool RemoveComponent(const char* title, Entity& entity);
		/*template<typename T, typename ...Args, typename R = std::result_of<T(Args&&...)>::type>
		auto ShowWindowBar(const char* title, T callback, Args&& ...args)
		{
			R value;
			if (ImGui::Begin(title))
				value = callback(std::forward<Args>(args)...);
			ImGui::End();
			return value;
		}*/

	};

	template<typename Callback, typename ...Args>
	inline void ImGuiLayer::ShowWindowBar(const char* title, Callback callback, Args && ...args)
	{
		if (ImGui::Begin(title))
			std::invoke(callback, std::forward<Args>(args)...);
		ImGui::End();
	}
	template<typename Comp, typename Call, typename ...Args>
	inline void ImGuiLayer::DrawComponent(const char* title, Entity& entity, Call callback, Args && ...args)
	{
		if (entity.IsValid())
		{
			if (entity.HasComponent<Comp>())
			{
				ImGui::AlignTextToFramePadding();
				if (ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_KeppFramedWhenOpen))
				{
					if (!RemoveComponent<Comp>(title, entity))
						std::invoke(callback, std::forward<Args>(args)...);
						//callback(std::forward<Args>(args)...);
					ImGui::TreePop();
				}
			}
		}
	}
	template<typename Comp>
	inline bool ImGuiLayer::RemoveComponent(const char* title, Entity& entity)
	{
		if (entity.IsValid())
		{
			if (entity.HasComponent<Comp>())
			{
				std::string id = "RemoveComponentPopup##" + entity;
				std::string lable("Remove " + std::string(title));
				if (ImGui::BeginPopupContextItem())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0, 0, 1));
					if (ImGui::MenuItem(lable.c_str()))
					{
						entity.RemoveComponent<Comp>();
						ImGui::PopStyleColor();
						ImGui::EndPopup();

						return true; // has been removed
					}
					else
					{
						ImGui::PopStyleColor();
						ImGui::EndPopup();

						return false; // hasn't been removed
					}
				}
			}
		}

		return false;
	}
}




