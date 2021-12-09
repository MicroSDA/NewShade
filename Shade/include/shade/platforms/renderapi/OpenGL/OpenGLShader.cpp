#include "shade_pch.h"
#include "OpenGLShader.h"

shade::OpenGLShader::OpenGLShader(const std::string& filePath)
{
	m_Path = filePath;
	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		if (Deserialize(file))
		{
			m_Program = CreateProgram();
		}
		else
			SHADE_CORE_WARNING("Failed to deserialize source file :'{0}'.", filePath);
	}
	else
		SHADE_CORE_WARNING("Failed to open shader source file :'{0}'.", filePath);
}

shade::OpenGLShader::~OpenGLShader()
{
	// Delete program and shader
	UnBind();
	for (unsigned int i = 0; i < m_Shaders.size(); i++) {
		glDeleteShader(m_Shaders[i]);
	}
	glDeleteProgram(m_Program);
}

void shade::OpenGLShader::Bind() const
{
	glUseProgram(m_Program);
}

void shade::OpenGLShader::UnBind() const
{
	glUseProgram(0);
}

std::uint32_t shade::OpenGLShader::GetProgram()
{
	return m_Program;
}

void shade::OpenGLShader::SendInt(const std::string& name, const int& value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void shade::OpenGLShader::SendInt2(const std::string& name, const int* value)
{
	glUniform2i(GetUniformLocation(name), value[0], value[1]);
}

void shade::OpenGLShader::SendInt3(const std::string& name, const int* value)
{
	glUniform3i(GetUniformLocation(name), value[0], value[1], value[2]);
}

void shade::OpenGLShader::SendInt4(const std::string& name, const int* value)
{
	glUniform4i(GetUniformLocation(name), value[0], value[1], value[2], value[3]);
}

void shade::OpenGLShader::SendFloat(const std::string& name, const float& value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void shade::OpenGLShader::SendFloat2(const std::string& name, const float* value)
{
	glUniform2f(GetUniformLocation(name), value[0], value[1]);
}

void shade::OpenGLShader::SendFloat3(const std::string& name, const float* value)
{
	glUniform3f(GetUniformLocation(name), value[0], value[1], value[2]);
}

void shade::OpenGLShader::SendFloat4(const std::string& name, const float* value)
{
	glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3]);
}

void shade::OpenGLShader::SendMat3(const std::string& name, const bool& isTransponse, const float* value)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, isTransponse, value);
}

void shade::OpenGLShader::SendMat4(const std::string& name, const bool& isTransponse, const float* value)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, isTransponse, value);
}

void shade::OpenGLShader::SelectSubrutine(const std::string& sUniformName, const std::string& subrutine, const Shader::Type& type)
{
	auto subrutineIndex = glGetSubroutineIndex(m_Program, ToOpenGLShaderType(type), subrutine.c_str());
	if (subrutineIndex != GL_INVALID_INDEX)
	{
		auto uniformIndex = glGetSubroutineUniformLocation(m_Program, ToOpenGLShaderType(type), sUniformName.c_str());

		if (uniformIndex >= 0)
		{
			switch (type)
			{
			case Shader::Type::Vertex:   if (uniformIndex < m_VertexSubrIndices.size())   m_VertexSubrIndices[uniformIndex]		= subrutineIndex; break;
			case Shader::Type::Fragment: if (uniformIndex < m_FragemntSubrIndices.size()) m_FragemntSubrIndices[uniformIndex]	= subrutineIndex; break;
			case Shader::Type::Geometry: if (uniformIndex < m_GeometrySubrIndices.size()) m_GeometrySubrIndices[uniformIndex]	= subrutineIndex; break;
			case Shader::Type::Compute:  if (uniformIndex < m_ComputeSubrIndices.size())  m_ComputeSubrIndices[uniformIndex]	= subrutineIndex; break;
			default: SHADE_CORE_WARNING("Undefined shader type for subrutine selection! Type: '{0}.'", (std::uint32_t)type) break;
			}
		}
		else
			SHADE_CORE_WARNING("Subrutine invalid uniform index for : {0}.", sUniformName);
	}
	else
		SHADE_CORE_WARNING("Subrutine invalid index for : {0} : {1}.", sUniformName, subrutine);
}

void shade::OpenGLShader::ExecuteSubrutines()
{
	// Update subrutines
	if (m_VertexSubrIndices.size())		glUniformSubroutinesuiv(GL_VERTEX_SHADER,   m_VertexSubrIndices.size(),   m_VertexSubrIndices.data());
	if (m_FragemntSubrIndices.size())	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, m_FragemntSubrIndices.size(), m_FragemntSubrIndices.data());
	if (m_GeometrySubrIndices.size())	glUniformSubroutinesuiv(GL_GEOMETRY_SHADER, m_GeometrySubrIndices.size(), m_GeometrySubrIndices.data());
	if (m_ComputeSubrIndices.size())	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, m_ComputeSubrIndices.size(), m_ComputeSubrIndices.data());
}

void shade::OpenGLShader::DispatchCompute(const std::uint32_t& x, const std::uint32_t& y, const std::uint32_t& z)
{
	// TODO memory barier
	glDispatchCompute(x, y, z);
}

GLuint shade::OpenGLShader::CreateProgram()
{
	GLuint program = glCreateProgram();

	for (const auto& shader : m_SourceCode)
	{
		GLuint _shader = CreateShader(shader.second, ToOpenGLShaderType(shader.first));
		glAttachShader(program, _shader);
		m_Shaders.push_back(_shader);
	}

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: Shader program linkin faild");
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: Shader program validate faild");


	// Getting active subrutines;
	GLint vS_Count = 0, fS_Count = 0, gS_Count = 0, cS_Count = 0;
	glGetProgramStageiv(program, GL_VERTEX_SHADER,   GL_ACTIVE_SUBROUTINE_UNIFORMS, &vS_Count);
	glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &fS_Count);
	glGetProgramStageiv(program, GL_GEOMETRY_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &gS_Count);
	glGetProgramStageiv(program, GL_COMPUTE_SHADER,  GL_ACTIVE_SUBROUTINE_UNIFORMS, &cS_Count);

	m_VertexSubrIndices.resize(vS_Count);
	m_FragemntSubrIndices.resize(fS_Count);
	m_GeometrySubrIndices.resize(gS_Count);
	m_ComputeSubrIndices.resize(cS_Count);

	return program;
}

GLuint shade::OpenGLShader::CreateShader(const std::string& source, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	if (shader == 0)
	{
		SHADE_CORE_WARNING("Shader creation failed!")
	}

	const GLchar* shaderSourceSrtings[1];
	GLint shaderSourceStringLengths[1];

	shaderSourceSrtings[0] = source.c_str();
	shaderSourceStringLengths[0] = source.length();


	glShaderSource(shader, 1, shaderSourceSrtings, shaderSourceStringLengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed");

	return shader;
}

void shade::OpenGLShader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
	{
		glGetProgramiv(shader, flag, &success);
	}
	else
	{
		glGetShaderiv(shader, flag, &success);
	}

	if (success == GL_FALSE)
	{
		if (isProgram)
		{
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		}
		else
		{
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}

		SHADE_CORE_WARNING("{0}: {1}", errorMessage, error)
	}
}

GLenum shade::OpenGLShader::ToOpenGLShaderType(const Type& type)
{
	switch (type)
	{
	case Type::Vertex:		return GL_VERTEX_SHADER;
	case Type::Fragment:	return GL_FRAGMENT_SHADER;
	case Type::Geometry:	return GL_GEOMETRY_SHADER;
	case Type::Compute:	    return GL_COMPUTE_SHADER;
	default:
		SHADE_CORE_WARNING("Undefined shader type!"); return -1;
	}
}

void shade::OpenGLShader::Recompile()
{
	UnBind();
	for (unsigned int i = 0; i < m_Shaders.size(); i++) {
		glDeleteShader(m_Shaders[i]);
	}
	glDeleteProgram(m_Program);

	std::ifstream file;
	file.open(m_Path, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		if (Deserialize(file))
		{
			m_Program = CreateProgram();
		}
		else
			SHADE_CORE_WARNING("Failed to deserialize source file :'{0}'.", m_Path);
	}
	else
		SHADE_CORE_WARNING("Failed to open shader source file :'{0}'.", m_Path);
}

int shade::OpenGLShader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocation.find(name) != m_UniformLocation.end())
	{
		return m_UniformLocation[name];
	}
	else
	{
		GLint _Location = glGetUniformLocation(m_Program, name.c_str());
		m_UniformLocation[name] = _Location;
		return _Location;
	}
}

bool shade::OpenGLShader::Serialize(std::ostream& stream) const
{
	return false;
}

bool shade::OpenGLShader::Deserialize(std::istream& stream) // Need to move in shader not opengl shader
{
	stream.seekg(0, std::ios::end);
	size_t size = stream.tellg();
	stream.seekg(0, std::ios::beg);
	if (size != -1)
	{
		std::string source;
		source.resize(size);
		util::Binarizer::Read(stream, source[0], size);
		if (!Parse(source))
			return false;
	}
	else
	{
		SHADE_CORE_WARNING("Failed to deserialize shader source file, size is 0.");
		return false;
	}

	return true;
}

bool shade::OpenGLShader::Serialize() const
{
	return false;
}

bool shade::OpenGLShader::Deserialize()
{
	return false;
}
