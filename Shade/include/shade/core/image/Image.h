#pragma once
#include "shade/core/utils/Binarizer.h"
#include "shade/core/utils/Log.h"

namespace shade
{
	struct ImageData
	{
		std::uint8_t*  Data = nullptr;
		std::uint16_t  Width = 0;
		std::uint16_t  Height = 0;
		std::uint8_t   MipMapCount = 1;
		std::uint32_t  Size = 0;
		enum class DXTCompression : std::uint32_t
		{
			Undefined = 0,
			DXT1 = 827611204,
			DXT3 = 861165636,
			DXT5 = 894720068,
			DXT10 = 808540228,
		} Compression = DXTCompression::Undefined;

		void Delete()
		{
			if (Data != nullptr)
			{
				delete[] Data;
				Data = nullptr;
			}
		}
	};
	class Image
	{
	private:
		struct Header
		{
			std::uint32_t Magic;
			std::uint32_t Size;
			std::uint32_t Flags;
			std::uint32_t Height;
			std::uint32_t Width;
			std::uint32_t PitchOrLinearSize;
			std::uint32_t Depth;
			std::uint32_t MipMapCount;
			std::uint32_t Reserved1[11];
			struct DdsPixelFormat
			{
				std::uint32_t dwSize;
				std::uint32_t dwFlags;
				std::uint32_t dwFourCC;
				std::uint32_t dwRGBBitCount;
				std::uint32_t dwRBitMask;
				std::uint32_t dwGBitMask;
				std::uint32_t dwBBitMask;
				std::uint32_t dwABitMask;
			} Dspf;
			std::uint32_t Caps;
			std::uint32_t Caps2;
			std::uint32_t Caps3;
			std::uint32_t Caps4;
			std::uint32_t Reserved2;
		};

	public:
		static ImageData LoadFromFile(const std::string& filePath);
		static ImageData LoadFromStream(std::istream& stream);
	private:
		static void ReadHeader(std::istream& stream, Header& header);
	private:
	};

}