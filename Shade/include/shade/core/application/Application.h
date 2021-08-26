#pragma once
#include "shade/config/API.h"
#include "shade/core/events/ApplicationEvent.h"
#include "shade/core/events/KeyEvent.h"
#include "shade/core/events/KeyCodes.h"
#include "shade/core/events/Input.h"
#include "shade/core/events/MouseEvent.h"
#include "shade/core/window/Window.h"
#include "shade/core/entities/EntitiesDocker.h"
#include "shade/core/entities/Entity.h"
#include "shade/core/time/Timer.h"
#include "shade/core/components/Components.h"
#include "shade/core/scene/Scene.h"


#undef CreateWindow

int main(int argc, char* argv[]);

namespace shade
{
	class Layer;

	class SHADE_API Application : public EntitiesDocker
	{
	public:
		Application(int argc, char* argv[]);
		virtual ~Application(); 
		static  Application& Get();
		Unique<Window>& CreateWindow(const WindowConfig& config = WindowConfig());
		Unique<Window>& GetWindow();
		void						SetCurrentScene(const Shared<Scene>& scene);
		Shared<Scene>				GetCurrentScene();
	private:
	//Need make private and specific friend for handle this
		void Start();
		void Quit();
	protected:
		virtual void				OnCreate() = 0;
		virtual void				OnUpdate(const shade::Timer& timer) = 0;
	protected:
		Shared<Scene>				m_CurrentScene;
		Shared<Scene>				CreateScene(const std::string& name);
		Layer*						GetLayer(const std::string& name);
		void						DeleteCurrentScene();
		template<typename T>
		T*			                CreateLayer(const std::string& name);
		template<typename T>
		auto						GetLayer(const std::string& name)->T*;
		bool						DeleteLayer(const std::string& name);
	private:
		void						OnEvent(shade::Event& event);
		void						NativeScriptsUpdate(const shade::Timer& deltaTime);
		Unique<Window>				m_Window;
		static Application*			m_pInstance;
		bool						m_IsQuitRequested = false;
		std::vector<Layer*>	        m_Layers;
		shade::Timer				m_DeltaTime;
		friend int ::main(int argc, char* argv[]);
	};

	Application* CreateApplication(int argc, char* argv[]);
	template<typename T>
	inline T* Application::CreateLayer(const std::string& name)
	{
		for (auto& layer : m_Layers)
		{
			if (layer->GetName() == name)
			{
				SHADE_CORE_WARNING("Layer '{0}' allready exist!", name);
				return nullptr;
			}

		}

		auto pLayer = new T(name);
		pLayer->OnCreate();
		m_Layers.push_back(pLayer);
		return pLayer;
	}
	template<typename T>
	inline auto Application::GetLayer(const std::string& name) -> T*
	{
		for (auto& layer : m_Layers)
		{
			if (layer->GetName() == name)
				return static_cast<T*>(layer);
		}
		
		SHADE_CORE_WARNING("Layer '{0}' has not been found!", name);
		return nullptr;
	}
}