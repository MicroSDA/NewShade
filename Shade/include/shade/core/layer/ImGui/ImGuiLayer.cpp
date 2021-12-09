#include "shade_pch.h"
#include "ImGuiLayer.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
//#include "shade/core/layer/ImGui/ImGuiCurve.h"
#include <ImGui/imgui_internal.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

shade::ImGuiLayer::ImGuiLayer(const std::string& name) : 
	shade::Layer(name)
{
	auto context = ImGui::CreateContext();
	ImGui::SetCurrentContext(context);
	//SetupImGuiStyle(true, false);
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.DisplaySize = ImVec2((float)shade::Application::Get().GetWindow()->GetWidth(), (float)shade::Application::Get().GetWindow()->GetHeight());
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		//style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	auto& app = shade::Application::Get();
	auto window = shade::Application::Get().GetWindow()->GetNativeWindow();

	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
	ImGui_ImplOpenGL3_Init("#version 440");


	m_WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	m_WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	m_WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	m_DockSpaceFlags = ImGuiDockNodeFlags_None;

	m_Viewport = ImGui::GetMainViewport();
}

shade::ImGuiLayer::~ImGuiLayer()
{
	ImGui::DestroyContext();
}

void shade::ImGuiLayer::ShowDemoWindow()
{
	ImGui::ShowDemoWindow();
}

void shade::ImGuiLayer::OnRenderBegin()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::SetNextWindowPos(m_Viewport->WorkPos);
	ImGui::SetNextWindowSize(m_Viewport->WorkSize);
	ImGui::SetNextWindowViewport(m_Viewport->ID);
}

void shade::ImGuiLayer::OnRenderEnd()
{
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2((float)app.GetWindow()->GetWidth(), (float)app.GetWindow()->GetHeight());

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void shade::ImGuiLayer::OnImGuiEvent(shade::Event& event)
{

}

bool shade::ImGuiLayer::InputText(const char* title, char* buffer, std::size_t buffer_size)
{
	ImGui::Text(title);
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
	std::stringstream _title;
	_title << "##" << title;
	bool isInput = ImGui::InputText(_title.str().c_str(), buffer, buffer_size);
	ImGui::PopItemWidth();
	return isInput;
}

void shade::ImGuiLayer::FpsOverlay(ImGuiViewport* viewport)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x + 12, ImGui::GetWindowPos().y + 32 }, ImGuiCond_Always);
	if (ImGui::Begin("FPS", nullptr, window_flags))
	{
		ImGui::Text("Application average %.1f ms/frame (%.0f FPS)", 1000.0f / io.Framerate, io.Framerate);
	}
	ImGui::End();
}

void shade::ImGuiLayer::DrawImage(const std::uint32_t& renderId, const float& width, const float& height, const bool& tooltip)
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2   pos = ImGui::GetCursorScreenPos();

	ImTextureID textureId = reinterpret_cast<void*>(renderId);
	ImGui::Image(textureId, ImVec2{ width, height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	if (tooltip)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			float region_sz = 100.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
			float zoom = 5.0f;
			if (region_x < 0.0f) { region_x = 0.0f; }
			else if (region_x > width - region_sz) { region_x = width - region_sz; }
			if (region_y < 0.0f) { region_y = 0.0f; }
			else if (region_y > height - region_sz) { region_y = height - region_sz; }

			ImGui::Image(textureId, ImVec2(region_sz * zoom, region_sz * zoom), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::EndTooltip();
		}
	}
}

bool shade::ImGuiLayer::DrawVec3F(const char* title, float* data, const float& resetValue, const float& min, const float& max, const float& colw1)
{
	bool isUsed = false;

	ImGui::PushID(title);
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, colw1);
	ImGui::Text(title);
	ImGui::NextColumn();

	/*if (ImGui::Button("X")) { data[0] = resetValue; isUsed = true; }

	const ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 label_size = ImGui::CalcTextSize("X", NULL, true);
	ImVec2 button_size = ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
	float width = (ImGui::GetContentRegionAvailWidth() - ((colw1 + style.FramePadding.x * 3) / 2) - ((button_size.x - style.FramePadding.x) * 3)) / 3;

	ImGui::SameLine();
	ImGui::PushItemWidth(width);
	if (ImGui::DragFloat("##X", &data[0], 0.01f, min, max, "%.2f"))
		isUsed = true;

	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("Y")) { data[1] = resetValue; isUsed = true; } ImGui::SameLine();
	ImGui::PushItemWidth(width);
	if (ImGui::DragFloat("##Y", &data[1], 0.01f, min, max, "%.2f"))
		isUsed = true;

	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("Z")) { data[2] = resetValue; isUsed = true; } ImGui::SameLine();
	ImGui::PushItemWidth(width);
	if (ImGui::DragFloat("##Z", &data[2], 0.01f, min, max, "%.2f"))
		isUsed = true;

	ImGui::PopItemWidth();*/

	const ImGuiStyle& style = ImGui::GetStyle();
	//ImVec2 label_size		= ImGui::CalcTextSize("X", NULL, true);
	//ImVec2 button_size		= ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.ItemSpacing.x, label_size.y + style.ItemSpacing.y);
	float width				= (ImGui::GetContentRegionAvailWidth() / 3.0) - ((style.ItemSpacing.x) * 6.0);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.753f, 0.000f, 0.099f, 0.709f });	ImGui::Button("X"); ImGui::PopStyleColor();
	ImGui::SameLine();  ImGui::PushItemWidth(width); ImGui::DragFloat("##X", &data[0]); ImGui::PopItemWidth(); ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.000f, 0.698f, 0.008f, 0.709f });	ImGui::Button("Y"); ImGui::PopStyleColor();
	ImGui::SameLine();  ImGui::PushItemWidth(width); ImGui::DragFloat("##Y", &data[1]); ImGui::PopItemWidth(); ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.257f, 0.542f, 0.852f, 0.709f });	ImGui::Button("Z"); ImGui::PopStyleColor();
    ImGui::SameLine();  ImGui::PushItemWidth(width); ImGui::DragFloat("##Z", &data[2]); ImGui::PopItemWidth();

	ImGui::Columns(1);
	ImGui::PopID();

	return isUsed;
}
bool shade::ImGuiLayer::DrawFlaot(const char* lable, float* data, const float& reset, const float& min, const float& max, const float& cw1, const float& cw2)
{
	ImGui::PushID(lable);
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, cw1);
	if (cw2 > 0.0f)
		ImGui::SetColumnWidth(1, cw2);

	ImGui::Text(lable);
	ImGui::NextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
	bool isUsed = ImGui::DragFloat("##value", data, 0.001f, min, max, "%.5f");
	ImGui::PopItemWidth();
	ImGui::Columns(1);
	ImGui::PopID();

	return isUsed;
}
bool shade::ImGuiLayer::DrawInt(const char* lable, int* data, const int& reset, const int& min, const int& max, const float& cw1, const float& cw2)
{
	ImGui::PushID(lable);
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, cw1);
	if (cw2 > 0.0f)
		ImGui::SetColumnWidth(1, cw2);

	ImGui::Text(lable);
	ImGui::NextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
	bool isUsed = ImGui::DragInt("##value", data, 0.05f, min, max, "%.d");
	ImGui::PopItemWidth();
	ImGui::Columns(1);
	ImGui::PopID();

	return isUsed;
}
bool shade::ImGuiLayer::DrawColor3(const char* title, float* data, const float& cw1, const float& cw2)
{
	ImGui::PushID(title);
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, cw1);
	if (cw2 > 0.0f)
		ImGui::SetColumnWidth(1, cw2);

	ImGui::Text(title);
	ImGui::NextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
	bool isUsed = ImGui::ColorEdit3("##color", data);
	ImGui::PopItemWidth();
	ImGui::Columns(1);
	ImGui::PopID();

	return isUsed;
}
bool shade::ImGuiLayer::DrawImGuizmo(glm::mat4& transform, const Shared<Camera>& camera, const ImGuizmo::OPERATION& operation, const float& x, const float& y, const float& width, const float& height)
{
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(x, y, width, height);

	auto cameraView = camera->GetView();
	auto cameraProjection = camera->GetProjection();

	ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), operation, ImGuizmo::WORLD, glm::value_ptr(transform));

	if (ImGuizmo::IsUsing())
		return true;
	else
		return false;
}

int shade::ImGuiLayer::DrawCurve(const std::string& label, const float* values, int points_count, const ImVec2& editor_size)
{
	ImGui::PlotLines(std::string("##" + label).c_str(), values, points_count, 0, label.c_str(), 0.0f, FLT_MAX, editor_size, 4);
	return 0;
}

bool shade::ImGuiLayer::DrawButtonCol(const char* cw1Lable, const char* buttonLable, const float& cw1, const float& cw2)
{
	ImGui::PushID(cw1Lable);
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, cw1);
	if (cw2 > 0.0f)
		ImGui::SetColumnWidth(1, cw2);

	ImGui::Text(cw1Lable);
	ImGui::NextColumn();
	bool isUsed = ImGui::Button(buttonLable, ImVec2 { ImGui::GetContentRegionAvailWidth() + ImGui::GetStyle().ItemSpacing.x,0});
	ImGui::Columns(1);
	ImGui::PopID();

	return isUsed;
}

bool shade::ImGuiLayer::DrawButtonTrinagle(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	
	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderNavHighlight(bb, id);
	//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	
	ImVec2 _Points[3];
	_Points[0] = bb.Min;
	_Points[1] = { bb.Max.x, bb.Max.y - size.y / 2 };
	_Points[2] = { bb.Min.x, bb.Max.y };
	/**/
	window->DrawList->AddConvexPolyFilled(_Points, 3, col);
    /**/

	if (g.LogEnabled)
		ImGui::LogSetNextTextDecoration("[", "]");
	//ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	// Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//    CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

bool shade::ImGuiLayer::DrawButtonSquare(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);


	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderNavHighlight(bb, id);
	//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	ImVec2 _Points[4];
	_Points[0] = bb.Min;
	_Points[1] = { bb.Max.x, bb.Min.y };
	_Points[2] = bb.Max;
	_Points[3] = { bb.Min.x, bb.Max.y };
	/**/
	window->DrawList->AddConvexPolyFilled(_Points, 4, col);
	/**/

	if (g.LogEnabled)
		ImGui::LogSetNextTextDecoration("[", "]");
	//ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	// Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//    CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

bool shade::ImGuiLayer::DrawButtonImage(const char* id, const Shared<Texture>& texture, const ImVec2& buttonSize, const ImVec2& imageSize, const ImVec2& start, int frame_padding, const ImVec4& tint_col)
{
	ImVec2 textureSize = { (float)texture->GetImageData().Width, (float)texture->GetImageData().Height };

	ImVec2 uv1 = ImVec2(start.x / textureSize.x, start.y / textureSize.y);
	ImVec2 uv2 = ImVec2((start.x + imageSize.x) / textureSize.x, (start.y + imageSize.y) / textureSize.y);

	// To make background transparent
	ImGui::PushID(id);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0, 0.0, 0.0, 0.0 });
	auto isUsed = ImGui::ImageButton(reinterpret_cast<void*>(texture->GetRenderID()), buttonSize, uv1, uv2, frame_padding, ImVec4(0, 0, 0, 0), tint_col);
	ImGui::PopStyleColor();
	ImGui::PopID();
	return isUsed;
}

void shade::ImGuiLayer::AddChild(const char* title, Entity& entity)
{
	if (entity.IsValid())
	{
		if (ImGui::MenuItem(title))
		{
			entity.AddChild(entity.GetDocker().CreateEntity());
		}
			
	}
	
}
