#include "shade_pch.h"
#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>

shade::OpenGLUniformBuffer::OpenGLUniformBuffer(const std::uint32_t& size, const std::uint32_t& binding)
{
	glCreateBuffers(1, &m_RenderID);
	glNamedBufferData(m_RenderID, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RenderID);
}

shade::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	glDeleteBuffers(1, &m_RenderID);
}

void shade::OpenGLUniformBuffer::SetData(const void* data, const std::uint32_t& size, const std::uint32_t& offset)
{
	glNamedBufferSubData(m_RenderID, offset, size, data);
}
