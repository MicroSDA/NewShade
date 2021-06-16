#pragma once
#include "shade/config/API.h"

namespace shade
{
	class SHADE_API Timer
	{
	public:
		Timer() = default;
		~Timer() = default;
		void Update();
		operator double() const { return m_DeltaTime * 0.001; } // milliseconds
	private:
		double m_TimeLast, m_TimeNow, m_DeltaTime;
	};
}