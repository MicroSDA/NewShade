#include "shade_pch.h"
#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

shade::OpenGLIndexBuffer::OpenGLIndexBuffer(const Index* indices, const std::uint32_t& count):
	m_Count(count)
{
	glGenBuffers(1, &m_RenderID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Index) * count, indices, GL_STATIC_DRAW); // Always dynamic, keep in mind
	m_Size = sizeof(Index) * count;
}

shade::OpenGLIndexBuffer::OpenGLIndexBuffer(const std::uint32_t& count):
	m_Count(count)
{
	glGenBuffers(1, &m_RenderID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Index) * count, nullptr, GL_DYNAMIC_DRAW); // Always dynamic, keep in mind
	m_Size = sizeof(Index) * count;
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

void shade::OpenGLIndexBuffer::SetData(const std::uint32_t* data, const uint32_t& size, const uint32_t& offset)
{
	// Im to sure about it, need to be tested 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
	Unbind();
}
