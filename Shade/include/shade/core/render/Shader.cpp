#include "shade_pch.h"
#include "Shader.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLShader.h"

shade::Shared<shade::Shader>shade::Shader::Create(const std::string& filePath)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLShader>(filePath);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

bool shade::Shader::Parse(const std::string& source)
{
	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration 
		if (eol == std::string::npos)
		{
			SHADE_CORE_WARNING("Error to parse shader source: Syntaxis error!");
			return false;
		}
		size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
		std::string type = source.substr(begin, eol - begin);
		auto _type = GetTypeFromString(type);
		if (_type == Type::Vertex || _type == Type::Fragment || _type == Type::Geometry || _type == Type::Compute)
		{
			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line

			if (nextLinePos == std::string::npos)
			{
				SHADE_CORE_WARNING("Error to parse shader source: Syntaxis error!");
				return false;
			}
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line
		
			std::string complited = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
			FindInclude(complited);
			m_SourceCode[_type] = complited;
		}
		else
		{
			SHADE_CORE_WARNING("Invalid shader type specified!");
			return false;
		}
	}

	//std::cout << m_SourceCode[Type::Fragment] << std::endl;
	return true;
}

shade::Shader::Type shade::Shader::GetTypeFromString(const std::string& type)
{
	if (type == "vertex")
		return Type::Vertex;
	if (type == "fragment")
		return Type::Fragment;
	if (type == "geometry")
		return Type::Geometry;
	if (type == "compute")
		return Type::Compute;

	SHADE_CORE_WARNING("Undefined shader type!");
	return Type::Undefined;
}

void shade::Shader::FindInclude(std::string& source)
{
	const char* includeToken = "#include";
	size_t includeTokenLength = strlen(includeToken);
	std::vector<std::tuple<size_t, size_t>> includes;

	size_t start = source.find(includeToken, 0); // Start of shader include declaration line

	while (start != std::string::npos)
	{
		size_t end = source.find_first_of("\r\n", start); // End of shader include declaration line
		if (end != std::string::npos)
		{
			size_t begin = start + includeTokenLength + 1; //Start of shader include path (after "#include " keyword)
			std::string path = source.substr(begin, end - begin);
			source.erase(source.find(std::string(includeToken) + ' ' + path), path.size() + includeTokenLength + 1);// Remove #include with path
			path.erase(0, 1);// Remove first "
			path.erase(path.size() - 1);// Remove last "

			std::ifstream include(path, std::ios::in | std::ios::binary);
			if (include.is_open())
			{
				include.seekg(0, std::ios::end);
				size_t size = include.tellg();
				include.seekg(0, std::ios::beg);
				if (size != -1)
				{
					std::string _source;
					_source.resize(size);
					util::Binarizer::Read(include, _source[0], size);

					const char* versionToken = "#version";
					size_t versionTokenLength = strlen(versionToken);
					start = source.find(versionToken, 0); //Start of shader version declaration line
					end = source.find_first_of("\r\n", start); //End of shader version declaration 

					if (end != std::string::npos)
					{
						begin = start + versionTokenLength; //Start of shader version (after "#version " keyword)
						FindInclude(_source);
						source.insert(end, "\n" + _source);
					}
					else
					{
						FindInclude(_source);
						source.insert(0, _source);
					}
				}
				else
				{
					SHADE_CORE_WARNING("Failed to open source include file :'{0}'.", path);
				}
			}
			else
			{
				SHADE_CORE_WARNING("Failed to open source include file :'{0}'.", path);
			}

			start = source.find(includeToken, 0); // Start of shader include declaration line
		}
	}
}

// Shader Library
std::unordered_map<std::string, shade::Shared<shade::Shader>> shade::ShadersLibrary::m_sLibrary;

shade::Shared<shade::Shader> shade::ShadersLibrary::Create(const std::string& name, const std::string& filePath)
{
	auto& shader = m_sLibrary.find(name);
	if (shader == m_sLibrary.end())
	{
		m_sLibrary[name] = Shader::Create(filePath);
		return m_sLibrary[name];
	}
	else
	{
		SHADE_CORE_WARNING("Shader '{0}' is already exist in library!", name);
		return nullptr;
	}
}

shade::Shared<shade::Shader> shade::ShadersLibrary::Get(const std::string& name)
{
	auto& shader = m_sLibrary.find(name);
	if (shader != m_sLibrary.end())
		return shader->second;
	else
	{
		SHADE_CORE_WARNING("Shader '{0}' doensn't exist in library!", name);
		return nullptr;
	}	
}

void shade::ShadersLibrary::Remove(const std::string& name)
{
	auto& shader = m_sLibrary.find(name);
	if (shader != m_sLibrary.end())
		m_sLibrary.erase(shader);
	else
		SHADE_CORE_WARNING("Shader '{0}' doensn't exist in library!", name);
}
std::unordered_map<std::string, shade::Shared<shade::Shader>>& shade::ShadersLibrary::GetLibrary()
{
	return m_sLibrary;
}