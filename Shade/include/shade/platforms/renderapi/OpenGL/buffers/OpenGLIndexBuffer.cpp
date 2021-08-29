#include "shade_pch.h"
#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

shade::OpenGLIndexBuffer::OpenGLIndexBuffer(const Index* indices, const std::uint32_t& count)
{
	glCreateBuffers(1, &m_RenderID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Index) * count, indices, GL_DYNAMIC_DRAW); // Always dynamic, keep in mind
}

shade::OpenGLIndexBuffer::OpenGLIndexBuffer(const std::uint32_t& count)
{
	glCreateBuffers(1, &m_RenderID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Index) * count, nullptr, GL_DYNAMIC_DRAW); // Always dynamic, keep in mind
}

shade::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_RenderID);
}

void shade::OpenGLIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
}

void shade::OpenGLIndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void shade::OpenGLIndexBuffer::SetData(const shade::Index* data, const uint32_t& count, const uint32_t& offset)
{
	// Im to sure about it, need to be tested 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, sizeof(shade::Index) * count, data);
	Unbind();
}

std::uint32_t shade::OpenGLIndexBuffer::GetCount() const
{
	return GetSize() / sizeof(shade::Index);
}

std::uint32_t shade::OpenGLIndexBuffer::GetSize() const
{
	Bind();
	GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	return size;
}
