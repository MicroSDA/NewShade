#include "Scripts.h"
#include "shade/core/events/Input.h"
#include "shade/core/application/Application.h"

extern "C"
{
	SHADE_SCRIPT_API shade::ScriptableEntity* Instantiate(int& argc, char* argv[])
	{
		return new Script();
	}
}

void Script::OnCreate()
{
	
}

void Script::OnDestroy()
{
}

void Script::OnUpdate(const shade::Timer& deltaTime)
{
	auto _Camera = GetComponent<shade::CameraComponent>();
	/* Need change this */
	if (_Camera->IsPrimary())
	{
		{
			// Movment
			if (shade::Input::IsKeyPressed(shade::Key::W))
				_Camera->MoveForward(m_MovementSpeed * deltaTime);
			if (shade::Input::IsKeyPressed(shade::Key::S))
				_Camera->MoveBack(m_MovementSpeed * deltaTime);

			if (shade::Input::IsKeyPressed(shade::Key::A))
				_Camera->MoveLeft(m_MovementSpeed * deltaTime);
			if (shade::Input::IsKeyPressed(shade::Key::D))
				_Camera->MoveRight(m_MovementSpeed * deltaTime);
		}
		{
			if (shade::Input::IsMouseButtonPressed(shade::Mouse::ButtonRight))
			{
				shade::Input::ShowMouseCursor(false);
				glm::ivec2 _DeltaPos = shade::Input::GetMousePosition();

				_DeltaPos = _DeltaPos - glm::ivec2(shade::Application::Get().GetWindow()->GetWidth() / 2,
					shade::Application::Get().GetWindow()->GetHeight() / 2);

				_Camera->RotateYaw(-(_DeltaPos.x * m_RotationSpeed) / 100); // No deltaTime here 
				_Camera->RotatePitch(-(_DeltaPos.y * m_RotationSpeed) / 100);

				shade::Input::SetMousePosition(
					shade::Application::Get().GetWindow()->GetWidth() / 2,
					shade::Application::Get().GetWindow()->GetHeight() / 2);
			}
			else
				shade::Input::ShowMouseCursor(true);

		}
	}
}
