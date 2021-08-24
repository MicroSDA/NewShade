#pragma once
#include "shade/core/layer/Layer.h"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

struct ImGuiViewport;

namespace shade
{

	class SHADE_API ColorChemeEditor
	{
	public:
		inline static int BackGroundColor = 0x25213100;
		inline static int TextColor = 0xF4F1DE00;
		inline static int MainColor = 0xDA115E00;
		inline static int MainAccentColor = 0x79235900;
		inline static int HighlightColor = 0xC7EF0000;

		inline static int Black = 0x00000000;
		inline static int White = 0xFFFFFF00;
		inline static int AlphaTransparent = 0x00;
		inline static int Alpha20 = 0x33;
		inline static int Alpha40 = 0x66;
		inline static int Alpha50 = 0x80;
		inline static int Alpha60 = 0x99;
		inline static int Alpha80 = 0xCC;
		inline static int Alpha90 = 0xE6;
		inline static int AlphaFull = 0xFF;

		static float GetR(int colorCode) { return (float)((colorCode & 0xFF000000) >> 24) / (float)(0xFF); }
		static float GetG(int colorCode) { return (float)((colorCode & 0x00FF0000) >> 16) / (float)(0xFF); }
		static float GetB(int colorCode) { return (float)((colorCode & 0x0000FF00) >> 8) / (float)(0xFF); }
		static float GetA(int alphaCode) { return ((float)alphaCode / (float)0xFF); }

		static ImVec4 GetColor(int c, int a = Alpha80) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
		static ImVec4 Darken(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f * p), c.w); }
		static ImVec4 Lighten(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f * p), c.w); }

		static ImVec4 Disabled(ImVec4 c) { return Darken(c, 0.6f); }
		static ImVec4 Hovered(ImVec4 c) { return Lighten(c, 0.2f); }
		static ImVec4 Active(ImVec4 c) { return Lighten(ImVec4(c.x, c.y, c.z, 1.0f), 0.1f); }
		static ImVec4 Collapsed(ImVec4 c) { return Darken(c, 0.2f); }

		static void SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor)
		{
			BackGroundColor = backGroundColor;
			TextColor = textColor;
			MainColor = mainColor;
			MainAccentColor = mainAccentColor;
			HighlightColor = highlightColor;
		}
		static void ApplyTheme()
		{
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text] = GetColor(TextColor);
			colors[ImGuiCol_TextDisabled] = Disabled(colors[ImGuiCol_Text]);
			colors[ImGuiCol_WindowBg] = GetColor(BackGroundColor);
			colors[ImGuiCol_ChildBg] = GetColor(Black, Alpha20);
			colors[ImGuiCol_PopupBg] = GetColor(BackGroundColor, Alpha90);
			colors[ImGuiCol_Border]  = Lighten(GetColor(BackGroundColor), 0.1f);
			colors[ImGuiCol_BorderShadow] = GetColor(Black);
			colors[ImGuiCol_FrameBg] = GetColor(MainAccentColor, Alpha40);
			colors[ImGuiCol_FrameBgHovered] = Hovered(colors[ImGuiCol_FrameBg]);
			colors[ImGuiCol_FrameBgActive] = Active(colors[ImGuiCol_FrameBg]);
			colors[ImGuiCol_TitleBg] = GetColor(BackGroundColor, Alpha80);
			colors[ImGuiCol_TitleBgActive] = Active(colors[ImGuiCol_TitleBg]);
			colors[ImGuiCol_TitleBgCollapsed] = Collapsed(colors[ImGuiCol_TitleBg]);
			colors[ImGuiCol_MenuBarBg] = Darken(GetColor(BackGroundColor), 0.2f);
			colors[ImGuiCol_ScrollbarBg] = Lighten(GetColor(BackGroundColor, Alpha50), 0.4f);
			colors[ImGuiCol_ScrollbarGrab] = Lighten(GetColor(BackGroundColor), 0.3f);
			colors[ImGuiCol_ScrollbarGrabHovered] = Hovered(colors[ImGuiCol_ScrollbarGrab]);
			colors[ImGuiCol_ScrollbarGrabActive] = Active(colors[ImGuiCol_ScrollbarGrab]);
			colors[ImGuiCol_CheckMark] = GetColor(HighlightColor);
			colors[ImGuiCol_SliderGrab] = GetColor(HighlightColor);
			colors[ImGuiCol_SliderGrabActive] = Active(colors[ImGuiCol_SliderGrab]);
			colors[ImGuiCol_Button] = GetColor(MainColor, Alpha80);
			colors[ImGuiCol_ButtonHovered] = Hovered(colors[ImGuiCol_Button]);
			colors[ImGuiCol_ButtonActive] = Active(colors[ImGuiCol_Button]);
			colors[ImGuiCol_Header] = GetColor(MainColor, Alpha80);
			colors[ImGuiCol_HeaderHovered] = Hovered(colors[ImGuiCol_Header]);
			colors[ImGuiCol_HeaderActive] = Active(colors[ImGuiCol_Header]);
			colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
			colors[ImGuiCol_SeparatorHovered] = Hovered(colors[ImGuiCol_Separator]);
			colors[ImGuiCol_SeparatorActive] = Active(colors[ImGuiCol_Separator]);
			colors[ImGuiCol_ResizeGrip] = GetColor(MainColor, Alpha20);
			colors[ImGuiCol_ResizeGripHovered] = Hovered(colors[ImGuiCol_ResizeGrip]);
			colors[ImGuiCol_ResizeGripActive] = Active(colors[ImGuiCol_ResizeGrip]);
			colors[ImGuiCol_Tab] = GetColor(MainColor, Alpha60);
			colors[ImGuiCol_TabHovered] = Hovered(colors[ImGuiCol_Tab]);
			colors[ImGuiCol_TabActive] = Active(colors[ImGuiCol_Tab]);
			colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
			colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
			colors[ImGuiCol_DockingPreview] = Lighten(GetColor(MainAccentColor), 0.2f);
			colors[ImGuiCol_DockingEmptyBg] = Lighten(colors[ImGuiCol_HeaderActive], 0.6f);
			colors[ImGuiCol_PlotLines] = GetColor(HighlightColor);
			colors[ImGuiCol_PlotLinesHovered] = Hovered(colors[ImGuiCol_PlotLines]);
			colors[ImGuiCol_PlotHistogram] = GetColor(HighlightColor);
			colors[ImGuiCol_PlotHistogramHovered] = Hovered(colors[ImGuiCol_PlotHistogram]);
			colors[ImGuiCol_TextSelectedBg] = GetColor(HighlightColor, Alpha40);
			colors[ImGuiCol_DragDropTarget] = GetColor(HighlightColor, Alpha80);;
			colors[ImGuiCol_NavHighlight] = GetColor(White);
			colors[ImGuiCol_NavWindowingHighlight] = GetColor(White, Alpha80);
			colors[ImGuiCol_NavWindowingDimBg] = GetColor(White, Alpha20);
			colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha60);

			auto& style = ImGui::GetStyle();
			style.WindowMenuButtonPosition = ImGuiDir_Right;
			style.WindowPadding = ImVec2(6, 4);
			style.WindowRounding = 6.0f;
			style.FramePadding = ImVec2(5, 2);
			style.FrameRounding = 3.0f;
			style.ItemSpacing = ImVec2(7, 1);
			style.ItemInnerSpacing = ImVec2(1, 1);
			style.TouchExtraPadding = ImVec2(0, 0);
			style.IndentSpacing = 6.0f;
			style.ScrollbarSize = 12.0f;
			style.ScrollbarRounding = 16.0f;
			style.GrabMinSize = 20.0f;
			style.GrabRounding = 2.0f;
			style.WindowTitleAlign.x = 0.50f;
			style.FrameBorderSize = 0.0f;
			style.WindowBorderSize = 1.0f;
		}
	};

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
		bool DrawFlaot(const char* lable, float* data, const float& reset = 0.0f, const float& min = -FLT_MAX, const float& max = FLT_MAX, const float& cw1 = 80.0f, const float& cw2 = 0.0f);
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




