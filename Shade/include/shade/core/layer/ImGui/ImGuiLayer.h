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
		template<typename Callback, typename ...Args>
		void ShowWindowBarOverlay(const char* title, ImGuiViewport* veiwport, Callback callback, Args && ...args);
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
		bool DrawFlaot(const char* lable, float* data, const float& reset = 0.0f, const float& min = -FLT_MAX, const float& max = FLT_MAX, const float& cw1 = 100.0f, const float& cw2 = 0.0f);
		bool DrawInt(const char* lable,   int* data, const int& reset = 0.0f, const int& min = -FLT_MAX, const int& max = FLT_MAX, const float& cw1 = 100.0f, const float& cw2 = 0.0f);
		bool DrawColor3(const char* title, float* data, const float& cw1 = 80.0f, const float& cw2 = 0);
		bool DrawImGuizmo(glm::mat4& transform, const Shared<Camera>& camera, const ImGuizmo::OPERATION& operation, const float& x, const float& y, const float& width, const float& height);
		int  DrawCurve(const std::string& label, const float* values, int points_count, const ImVec2& editor_size);
		bool DrawButtonCol(const char* cw1Lable, const char* buttonLable, const float& cw1 = 80.0f, const float& cw2 = 0.0f);
		bool DrawButtonTrinagle(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags = ImGuiButtonFlags_None);
		bool DrawButtonSquare(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags = ImGuiButtonFlags_None);
		bool DrawButtonImage(const Shared<Texture>& texture, const ImVec2& buttonSize, const ImVec2& imageSize, const ImVec2& start = ImVec2(0,0), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
		/*ImVec2 start = {892, 614};
		ImVec2 size = { 26,  26 };
		ImVec2 textureSize = { (float)m_IconsTexture->GetImageData().Width, (float)m_IconsTexture->GetImageData().Height };

		ImVec2 uv1 = ImVec2(start.x / textureSize.x, start.y / textureSize.y);
		ImVec2 uv2 = ImVec2((start.x + size.x) / textureSize.x, (start.y + size.y) / textureSize.y);

		ImGui::ImageButton(reinterpret_cast<void*>(m_IconsTexture->GetRenderID()),
			ImVec2{ 26, 26 }, uv1, uv2);*/
		/*ImGui::Image(reinterpret_cast<void*>(m_IconsTexture->GetRenderID()),
			ImVec2{26, 26}, uv1, uv2);*/
		/*
		* 
		*/

	};

	template<typename Callback, typename ...Args>
	inline void ImGuiLayer::ShowWindowBar(const char* title, Callback callback, Args && ...args)
	{
		if (ImGui::Begin(title))
			std::invoke(callback, std::forward<Args>(args)...);
		ImGui::End();
	}
	template<typename Callback, typename ...Args>
	inline void ImGuiLayer::ShowWindowBarOverlay(const char* title, ImGuiViewport* veiwport, Callback callback, Args && ...args)
	{
		ImGui::SetNextWindowViewport(veiwport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
		ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x + 10.0f, ImGui::GetWindowPos().y + 30.0f }, ImGuiCond_Always);

		if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			std::invoke(callback, std::forward<Args>(args)...);
		}
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
				if (ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_Framed))
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




