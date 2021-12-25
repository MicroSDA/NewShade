#include "Scripts.h"
#include "shade/core/events/Input.h"
#include "shade/core/application/Application.h"

extern "C"
{	/* Only one arg is avalible at this moment !*/
	SHADE_SCRIPT_API shade::ScriptableEntity* Instantiate(int argc, void* argv[])
	{
		if(strcmp((char*)argv[0], "EditorCamera") == 0)
			return new EditorCameraScript();
	}
}

void EditorCameraScript::OnCreate()
{
	
}

void EditorCameraScript::OnDestroy()
{
}

void EditorCameraScript::OnUpdate(const shade::Timer& deltaTime)
{
	auto camera = GetComponent<shade::CameraComponent>();
	/* Need change this */
	if (camera->IsPrimary())
	{
		{
			// Movment
			if (shade::Input::IsKeyPressed(shade::Key::W))
				camera->MoveForward(m_MovementSpeed * deltaTime);
			if (shade::Input::IsKeyPressed(shade::Key::S))
				camera->MoveBackward(m_MovementSpeed * deltaTime);

			if (shade::Input::IsKeyPressed(shade::Key::A))
				camera->MoveLeft(m_MovementSpeed * deltaTime);
			if (shade::Input::IsKeyPressed(shade::Key::D))
				camera->MoveRight(m_MovementSpeed * deltaTime);

			if (shade::Input::IsKeyPressed(shade::Key::Q))
				camera->RotateZ(m_RotationSpeed / 100.f * deltaTime);
			if (shade::Input::IsKeyPressed(shade::Key::E))
				camera->RotateZ(-m_RotationSpeed / 100.f * deltaTime);

		}
		{
			if (shade::Input::IsMouseButtonPressed(shade::Mouse::ButtonRight))
			{
				shade::Input::ShowMouseCursor(false);
				glm::vec2 screenCenter(shade::Application::Get().GetWindow()->GetWidth() / 2, shade::Application::Get().GetWindow()->GetHeight() / 2);
				glm::vec2 mousePosition(shade::Input::GetMousePosition() - screenCenter);

				camera->Rotate(glm::vec3( mousePosition, 0.0f ) * m_RotationSpeed / 1000.f);
			
				shade::Input::SetMousePosition(screenCenter.x, screenCenter.y);
			}
			else
				shade::Input::ShowMouseCursor(true);

		}
	}
}
