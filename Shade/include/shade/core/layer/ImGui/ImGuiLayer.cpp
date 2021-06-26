#include "shade_pch.h"
#include "ImGuiLayer.h"


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

void shade::ImGuiLayer::OnCreate()
{
}

void shade::ImGuiLayer::OnUpdate(const shade::Shared<Scene>& scene, const shade::Timer& deltaTime)
{
}

void shade::ImGuiLayer::OnRenderBegin()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGuizmo::BeginFrame();

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

void shade::ImGuiLayer::OnDelete()
{
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
