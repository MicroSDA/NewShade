#pragma once
#include "shade_pch.h"
#include "shade/config/API.h"

namespace shade
{
	namespace util
	{
		class SHADE_API Binarizer
		{
		public:
			template<typename T>
			static auto Write(std::ostream& stream, const T& value, std::size_t size = 1) ->std::size_t
			{
				// Get bigin position of writing
				const auto begin = stream.tellp();
				stream.write(reinterpret_cast<const char*>(&value), sizeof(value) * size);
				// Return size 
				return static_cast<std::size_t>(stream.tellp() - begin);
			}
			static auto Write(std::ostream& stream, const std::string& value) ->std::size_t
			{
				// Get bigin position of writing
				const auto begin = stream.tellp();
				stream.write(value.data(), sizeof(char) * (value.size() + 1));
				// Return size 
				return static_cast<std::size_t>(stream.tellp() - begin);
			}
			template<typename T>
			static auto Read(std::istream& stream, T& value, std::size_t size = 1) ->std::size_t
			{
				// Get bigin position of reading
				const auto begin = stream.tellg();
				stream.read(reinterpret_cast<char*>(&value), sizeof(T) * size);
				// Return size 
				return static_cast<std::size_t>(stream.tellg() - begin);
			}
			static auto Read(std::istream& stream, std::string& value) ->std::size_t
			{
				// Get bigin position of reading
				const auto begin = stream.tellg();
				char buffer = ' ';
				while (buffer != '\0')
				{
					stream.read(&buffer, sizeof(char));
					value.push_back(buffer);
				}
				// Return size 
				return static_cast<std::size_t>(stream.tellg() - begin);
			}
		};

	}
}

