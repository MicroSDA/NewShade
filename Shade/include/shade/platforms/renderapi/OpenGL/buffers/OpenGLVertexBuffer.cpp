#include "shade_pch.h"
#include "OpenGLVertexBuffer.h"
#include <glad/glad.h>

shade::OpenGLVertexBuffer::OpenGLVertexBuffer(const std::uint32_t& size, const BufferType& type)
{
	glGenBuffers(1, &m_RenderID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<GLenum>(type));
	m_Size = size;
	m_Type = type;
}

shade::OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, const std::uint32_t& size, const BufferType& type)
{
	glGenBuffers(1, &m_RenderID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<GLenum>(type));
	m_Size = size;
	m_Type = type;
}

shade::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_RenderID);
}

void shade::OpenGLVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
}

void shade::OpenGLVertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void shade::OpenGLVertexBuffer::SetData(const void* data, const uint32_t& size, const uint32_t& offset)
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

const std::uint32_t& shade::OpenGLVertexBuffer::GetRenderID() const
{
	return m_RenderID;
}

void shade::OpenGLVertexBuffer::Copy(const Shared<VertexBuffer>& other, const std::uint32_t& size, const std::uint32_t & readOffset, const std::uint32_t& writeOffset)
{
	// Not sure if this works
	m_Layout = other->GetLayout();
	glBindBuffer(GL_COPY_READ_BUFFER, other->GetRenderID());
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_RenderID);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
}

void shade::OpenGLVertexBuffer::Resize(const std::uint32_t& size)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<GLenum>(m_Type));
	m_Size = size;
}
