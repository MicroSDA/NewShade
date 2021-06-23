#pragma once
#include "shade/config/API.h"
#include "shade/core/entities/Entity.h"
#include "shade/core/time/Timer.h"

namespace shade
{
	class SHADE_API ScriptableEntity
	{
	public:
		friend class  Scene;
		friend class  Application;
		friend class  NativeScript;

		virtual ~ScriptableEntity();
		void SetUpdate(const bool& isUpdate);
		const bool& IsUpdate() const;

		template<typename T>
		T& GetComponent();
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(const shade::Timer& deltaTime) {}
	private:
		bool m_IsUpdate = true;
		shade::Entity   m_Entity;
	};

	template<typename T>
	inline T& ScriptableEntity::GetComponent()
	{
		return m_Entity.GetComponent<T>();
	}
	template<typename T, typename ...Args>
	inline T& ScriptableEntity::AddComponent(Args && ...args)
	{
		return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
	}
}