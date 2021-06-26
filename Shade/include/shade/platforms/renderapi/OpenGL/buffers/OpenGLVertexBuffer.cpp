#include "shade_pch.h"
#include "OpenGLVertexBuffer.h"
#include <glad/glad.h>

shade::OpenGLVertexBuffer::OpenGLVertexBuffer(const std::uint32_t& size, const BufferType& type)
{
	glGenBuffers(1, &m_RenderID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<GLenum>(type));
}

shade::OpenGLVertexBuffer::OpenGLVertexBuffer(const Vertex3D* data, const std::uint32_t& size, const BufferType& type)
{
	glGenBuffers(1, &m_RenderID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * size, data, static_cast<GLenum>(type));
}

shade::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_RenderID);
}

void shade::OpenGLVertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
}

void shade::OpenGLVertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void shade::OpenGLVertexBuffer::SetData(const uint32_t& offset, const uint32_t& size, const Vertex3D* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	UnBind(); // ?
}

void shade::OpenGLVertexBuffer::SetLayout(const Layout& layout)
{
	m_Layout = layout;
}

const shade::VertexBuffer::Layout& shade::OpenGLVertexBuffer::GetLayout() const
{
	return m_Layout;
}
