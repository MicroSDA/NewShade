#pragma once
#include "shade/core/render/Shader.h"
#include <glad/glad.h>

namespace shade
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual std::uint32_t GetProgram() override;
		virtual void SendInt(const std::string& name, const int& value) override;
		virtual void SendInt2(const std::string& name, const int* value) override;
		virtual void SendInt3(const std::string& name, const int* value) override;
		virtual void SendInt4(const std::string& name, const int* value) override;
		virtual void SendFloat(const std::string& name, const float& value) override;
		virtual void SendFloat2(const std::string& name, const float* value) override;
		virtual void SendFloat3(const std::string& name, const float* value) override;
		virtual void SendFloat4(const std::string& name, const float* value) override;
		virtual void SendMat3(const std::string& name, const bool& isTransponse, const float* value) override;
		virtual void SendMat4(const std::string& name, const bool& isTransponse, const float* value) override;
		virtual void SelectSubrutine(const std::string& sUniformName, const std::string& subrutine, const Shader::Type& type) override;
		virtual void ExecuteSubrutines() override;
		virtual void DispatchCompute(const std::uint32_t& x, const std::uint32_t& y, const std::uint32_t& z) override;
		virtual int GetUniformLocation(const std::string& name) const override;

		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;

		virtual void Recompile() override;
	private:
		std::vector<GLuint> m_Shaders;
		GLuint				m_Program;

		GLuint CreateProgram();
		GLuint CreateShader(const std::string& source, GLenum shaderType);
		void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
		mutable std::unordered_map<std::string, GLint> m_UniformLocation; // Make it static ?
		GLenum ToOpenGLShaderType(const Type& type);
	};
}
	