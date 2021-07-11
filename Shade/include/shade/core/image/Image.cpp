#include "shade_pch.h"
#include "Image.h"

shade::ImageData shade::Image::LoadFromFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		return LoadFromStream(file);
	}
	else
	{
		SHADE_CORE_WARNING("Failed to open image file '{0}'.", filePath);
	}

	file.close();
	return ImageData();
}
shade::ImageData shade::Image::LoadFromStream(std::istream& stream)
{
	Header		header;
	ImageData	data;
	ReadHeader(stream, header);
	if (memcmp(&header.Magic, "DDS ", 4) == 0) // if magic is DDS
	{
		data.Width			= header.Width;
		data.Height			= header.Height;
		data.Compression	= static_cast<ImageData::DXTCompression>(header.Dspf.dwFourCC);
		data.MipMapCount	= header.MipMapCount;

		// Image size
		auto begin = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto end = stream.tellg();
		stream.seekg(begin);
		std::size_t imageSize = end - begin;

		// Create image buffer
		data.Data = new std::uint8_t[imageSize];
		// Read buffer
		shade::util::Binarizer::Read(stream, *data.Data, imageSize);
		return data;
	}
	else
	{
		SHADE_CORE_WARNING("Wrong image header !: {0}", header.Magic);
		return data;
	}
}
void shade::Image::ReadHeader(std::istream& stream, Header& header)
{
	shade::util::Binarizer::Read(stream, header);
}