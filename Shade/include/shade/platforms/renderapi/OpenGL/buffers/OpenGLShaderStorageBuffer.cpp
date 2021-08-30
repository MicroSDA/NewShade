#include "shade_pch.h"
#include "OpenGLShaderStorageBuffer.h"
#include <glad/glad.h>

shade::OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(const std::uint32_t& size, const std::uint32_t& binding)
{
	glCreateBuffers(1, &m_RenderID); 
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, m_RenderID, 0, size);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RenderID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	m_Binding = binding;
}

shade::OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
{
	glDeleteBuffers(1, &m_RenderID);
}

void shade::OpenGLShaderStorageBuffer::SetData(const void* data, const std::uint32_t& size, const std::uint32_t& offset)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RenderID);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void shade::OpenGLShaderStorageBuffer::Resize(const std::uint32_t& size)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RenderID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

std::uint32_t shade::OpenGLShaderStorageBuffer::GetSize() const
{
	GLint size = 0;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RenderID);
		glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
	return size;
}
