#pragma once
#include "shade/config/API.h"
#include "shade/core/time/Timer.h"
#include "shade/core/scene/Scene.h"
#include "shade/core/application/Application.h"
#include "shade/core/render/Render.h"
#include "shade/core/assets/AssetManager.h"
#include "shade/core/utils/FileDialog.h"

namespace shade
{
	class SHADE_API Layer
	{
	public:
		Layer(const std::string& name);
		virtual ~Layer();
		virtual void OnCreate()			= 0;
		virtual void OnUpdate(const shade::Shared<Scene>& scene, const shade::Timer& deltaTime) = 0;
		virtual void OnRenderBegin()	= 0;
		virtual void OnRender(const shade::Shared<Scene>& scene, const shade::Timer& deltaTime) = 0;
		virtual void OnRenderEnd()		= 0;
		virtual void OnDelete()			= 0;
		virtual void OnEvent(const shade::Shared<Scene>& scene, Event& event, const shade::Timer& deltaTime) = 0;
		void SetActive(const bool& isActive);
		void SetRender(const bool& isRender);
		void SetUpdate(const bool& isUpdate);

		const bool& IsActive() const;
		const bool& IsRender() const;
		const bool& IsUpdate() const;

		const std::string& GetName() const;
	private:
		std::string m_Name;
		bool m_IsActive = true;
		bool m_IsRender = true;
		bool m_IsUpdate = true;

		friend class Application;
	};

}