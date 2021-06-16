#pragma once
#include "shade/config/API.h"

namespace shade
{
	class SHADE_API Timer
	{
	public:
		Timer();
		~Timer() = default;
		void Update();
		operator float() const { return m_DeltaTime * 1000.0f; } // milliseconds
	private:
		float m_TimeLast, m_TimeNow, m_DeltaTime;
	};
}