#pragma once
#include "shade/config/API.h"

namespace shade
{
	class SHADE_API FileDialog
	{
	public:
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
		static std::filesystem::path SelectFolder(const char* filter);
	};
}


