#pragma once
#include "shade/core/layer/Layer.h"
#include "shade/core/layer/ImGui/ImGuiThemeEditor.h"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

struct ImGuiViewport;

namespace shade
{

	class SHADE_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name);
		virtual ~ImGuiLayer();
		// Inherited via Layer
		virtual void OnUpdate(const shade::Shared<Scene>& scene, const shade::Timer& deltaTime) = 0;
		virtual void OnRenderBegin() override;
		virtual void OnRenderEnd() override;

	private:

	protected:
		void OnImGuiEvent(shade::Event& event);
		int m_WindowFlags;
		int m_DockSpaceFlags;
		ImGuiViewport* m_Viewport;
	protected:
		void ShowDemoWindow();
		// Support functions
		template<typename Callback, typename ...Args>
		void ShowWindowBar(const char* title, Callback callback, Args&& ...args);
		template<typename Comp, typename Call, typename ...Args>
		void DrawComponent(const char* title, Entity& entity, Call callback, Args&& ...args);
		template<typename Call, typename ...Args>
		void DrawTreeNode(const char* title, Call callback, Args&& ...args);
		template<typename Comp, typename Call, typename ...Args>
		void AddComponent(const char* title, const bool& isMenu, Entity& entity, Call callback, Args&& ...args);
		template<typename Comp>
		bool RemoveComponent(const char* title, Entity& entity);
		template<typename Callback, typename ...Args>
		bool InputText(const char* title, char* buffer, std::size_t buffer_size, Callback callback, Args&& ...args);
		bool InputText(const char* title, char* buffer, std::size_t buffer_size);
		void FpsOverlay(ImGuiViewport* viewport);
		void DrawImage(const std::uint32_t& renderId, const float& width, const float& height, const bool& tooltip = false);
		bool DrawVec3F(const char* title, float* data, const float& resetValue = 0.0f, const float& min = -FLT_MAX, const float& max = FLT_MAX, const float& colw1 = 80.0f);
		bool DrawFlaot(const char* lable, float* data, const float& reset = 0.0f, const float& min = -FLT_MAX, const float& max = FLT_MAX, const float& cw1 = 140.0f, const float& cw2 = 0.0f);
		bool DrawColor3(const char* title, float* data, const float& cw1 = 80.0f, const float& cw2 = 0);
		bool DrawImGuizmo(glm::mat4& transform, const Shared<Camera>& camera, const ImGuizmo::OPERATION& operation, const float& x, const float& y, const float& width, const float& height);
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
	template<typename Call, typename ...Args>
	inline void ImGuiLayer::DrawTreeNode(const char* title, Call callback, Args && ...args)
	{
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_KeppFramedWhenOpen))
		{
			std::invoke(callback, std::forward<Args>(args)...);
			ImGui::TreePop();
		}
	}
	template<typename Comp, typename Call, typename ...Args>
	inline void ImGuiLayer::AddComponent(const char* title, const bool& isMenu, Entity& entity, Call callback, Args && ...args)
	{
		if (entity.IsValid())
		{
			if (!entity.HasComponent<Comp>())
			{
				if (isMenu)
				{
					if (ImGui::BeginMenu(title))
					{
						std::invoke(callback, std::forward<Args>(args)...);
						ImGui::EndMenu();
					}
				}
				else
				{
					if (ImGui::MenuItem(title))
						std::invoke(callback, std::forward<Args>(args)...);
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
	template<typename Callback, typename ...Args>
	inline bool ImGuiLayer::InputText(const char* title, char* buffer, std::size_t buffer_size, Callback callback, Args && ...args)
	{
		ImGui::Text(title);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
		std::stringstream _title;
		_title << "##" << title;
		bool isInput = ImGui::InputText(_title.str().c_str(), buffer, buffer_size);
		if (isInput)
			std::invoke(callback, std::forward<Args>(args)...);
		ImGui::PopItemWidth();
		return isInput;
	}
}




