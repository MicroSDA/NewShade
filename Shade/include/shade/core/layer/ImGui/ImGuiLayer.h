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
		template<typename Comp, typename Call, typename EditCall, typename ...Args>
		void DrawComponent2(const char* title, Entity& entity, Call callback, EditCall editCall, Args&& ...args);
		template<typename Comp>
		bool EditComponentButton(Entity& entity, const shade::Shared<Texture>& icon, ImVec4 imageProps, const bool& isTreeOpend);
		template<typename Call, typename ...Args>
		void DrawTreeNode(const char* title, Call callback, Args&& ...args);
		template<typename Comp, typename Call, typename ...Args>
		void AddComponent(const char* title, const bool& isMenu, Entity& entity, Call callback, Args&& ...args);
		template<typename Comp>
		bool RemoveComponent(Entity& entity, bool isOpen);
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
		bool DrawButtonImage(const char* id, const Shared<Texture>& texture, const ImVec2& buttonSize, const ImVec2& imageSize, const ImVec2& start = ImVec2(0,0), int frame_padding = -1, const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
		void AddChild(const char* title, Entity& entity);
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
		ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
		ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x + 10.0f, ImGui::GetWindowPos().y + 30.0f }, ImGuiCond_Always);
		//ImGui::SetNextWindowSize(ImVec2{ ImGui::GetWindowSize().x - 20.0f,0}, ImGuiCond_Always);

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
				/*ImGui::AlignTextToFramePadding();
				bool isOpen = ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed);

				std::invoke(buttonCall, isOpen, std::forward<Args>(args)...);

				if (isOpen)
				{
					std::invoke(callback, std::forward<Args>(args)...);
					ImGui::TreePop();
				}*/
			}
		}
	}
	template<typename Comp, typename Call, typename EditCall, typename ...Args>
	inline void ImGuiLayer::DrawComponent2(const char* title, Entity& entity, Call callback, EditCall editCall, Args&& ...args)
	{
		if (entity.IsValid())
		{
			if (entity.HasComponent<Comp>())
			{
				
				ImGui::AlignTextToFramePadding();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { ImGui::GetStyle().FramePadding.x , ImGui::GetStyle().FramePadding.y * 2.5f });
				bool isTreeOpen = ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed);
				ImGui::PopStyleVar();
				// If component has not been deleted
				if (!std::invoke(editCall, isTreeOpen))
				{
					if (isTreeOpen)
						std::invoke(callback, std::forward<Args>(args)...);
				}

				if (isTreeOpen)
					ImGui::TreePop();
			}
		}
	}
	template<typename Call, typename ...Args>
	inline void ImGuiLayer::DrawTreeNode(const char* title, Call callback, Args && ...args)
	{
		ImGui::AlignTextToFramePadding();
		//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		if (ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_KeppFramedWhenOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			std::invoke(callback, std::forward<Args>(args)...);
			ImGui::TreePop();
		}
		//ImGui::PopStyleVar();
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
	inline bool ImGuiLayer::EditComponentButton(Entity& entity, const shade::Shared<Texture>& icon, ImVec4 imageProps, const bool& isTreeOpend)
	{
		/* Return true only when component has been deleted */
		float buttonSize = ImGui::GetFrameHeight();

		ImGui::AlignTextToFramePadding();
		ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - buttonSize - ImGui::GetStyle().FramePadding.x + (isTreeOpend ? ImGui::GetStyle().IndentSpacing : 0.0f));
		// Create id for component
		std::string id = typeid(Comp).name();
		
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		if (DrawButtonImage(std::string("##Edit" + id).c_str(), icon, { buttonSize + 1.0f, buttonSize + 1.0f }, { imageProps.x, imageProps.y }, { imageProps.z, imageProps.w }, -1, ImGui::GetStyle().Colors[ImGuiCol_Text]))
			ImGui::OpenPopup(std::string("##EditComponentPopup" + id).c_str());
		ImGui::PopStyleColor();	//ImGui::PopStyleColor();
		if (ImGui::BeginPopup(std::string("##EditComponentPopup" + id).c_str()))
		{
			if (ImGui::MenuItem("Remove component"))
			{
				entity.RemoveComponent<Comp>();
				ImGui::EndPopup();
				return true;
			}
			else
			{
				ImGui::EndPopup();
				return false;
			}
				
			
		}

		return false;
	}
	template<typename Comp>
	inline bool ImGuiLayer::RemoveComponent(Entity& entity, bool isOpen)
	{
		if (entity.IsValid())
		{
			if (entity.HasComponent<Comp>())
			{
				std::string id = "RemoveComponentPopup##" + entity;
				std::string lable("Remove");
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




