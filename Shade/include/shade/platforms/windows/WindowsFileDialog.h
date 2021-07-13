#pragma once

namespace shade
{
	class WindowsFileDialog
	{
	public:
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
		static std::filesystem::path SelectFolder(const char* filter);
	};
}