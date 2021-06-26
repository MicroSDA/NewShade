#include "shade_pch.h"
#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

shade::OpenGLIndexBuffer::OpenGLIndexBuffer(std::uint32_t* indices, std::uint32_t count):
	m_Count(count)
{
	glGenBuffers(1, &m_RenderID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
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
