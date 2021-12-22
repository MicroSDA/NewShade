#include "shade_pch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace shade
{
	static GLenum ElementTypeToGLType(VertexBuffer::ElementType type)
	{
		switch (type)
		{
		case VertexBuffer::ElementType::Float:    return GL_FLOAT;
		case VertexBuffer::ElementType::Float2:   return GL_FLOAT;
		case VertexBuffer::ElementType::Float3:   return GL_FLOAT;
		case VertexBuffer::ElementType::Float4:   return GL_FLOAT;
		case VertexBuffer::ElementType::Mat3:     return GL_FLOAT;
		case VertexBuffer::ElementType::Mat4:     return GL_FLOAT;
		case VertexBuffer::ElementType::Int:      return GL_INT;
		case VertexBuffer::ElementType::Int2:     return GL_INT;
		case VertexBuffer::ElementType::Int3:     return GL_INT;
		case VertexBuffer::ElementType::Int4:     return GL_INT;
		case VertexBuffer::ElementType::Bool:     return GL_BOOL;
		default:
			SHADE_CORE_ERROR("Undefined element type"); return 0;
		}		
	}
}

shade::OpenGLVertexArray::OpenGLVertexArray()
{
	glGenVertexArrays(1, &m_RenderID);
}

shade::OpenGLVertexArray::~OpenGLVertexArray()
{
	//SHADE_CORE_TRACE("Vertex array id = {0} has been deleted.", m_RenderID);
	glDeleteVertexArrays(1, &m_RenderID);
}

void shade::OpenGLVertexArray::Bind() const
{
	glBindVertexArray(m_RenderID);
}

void shade::OpenGLVertexArray::UnBind() const
{
	glBindVertexArray(0);
}

void shade::OpenGLVertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
{
	if (!vertexBuffer->GetLayout().GetElements().size())
		SHADE_CORE_ERROR("Vertex buffer layout is 0!");

	Bind();
	vertexBuffer->Bind();

	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case VertexBuffer::ElementType::Float:
		case VertexBuffer::ElementType::Float2:
		case VertexBuffer::ElementType::Float3:
		case VertexBuffer::ElementType::Float4:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetTypeCount(),
				ElementTypeToGLType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case VertexBuffer::ElementType::Int:
		case VertexBuffer::ElementType::Int2:
		case VertexBuffer::ElementType::Int3:
		case VertexBuffer::ElementType::Int4:
		case VertexBuffer::ElementType::Bool:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribIPointer(m_VertexBufferIndex,
				element.GetTypeCount(),
				ElementTypeToGLType(element.Type),
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case VertexBuffer::ElementType::Mat3:
		case VertexBuffer::ElementType::Mat4:
		{
			uint8_t count = element.GetTypeCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					count,
					ElementTypeToGLType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		default:
			SHADE_CORE_ERROR("Undefined VertexBuffer element type!");
		}

	}

	m_VertexBuffers.push_back(vertexBuffer);
	UnBind();
}

void shade::OpenGLVertexArray::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RenderID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

const std::vector<shade::Shared<shade::VertexBuffer>>& shade::OpenGLVertexArray::GetVertexBuffers() const
{
	return m_VertexBuffers;
}

std::vector<shade::Shared<shade::VertexBuffer>>& shade::OpenGLVertexArray::GetVertexBuffers()
{
	return m_VertexBuffers;
}

const shade::Shared<shade::IndexBuffer>& shade::OpenGLVertexArray::GetIndexBuffer() const
{
	return m_IndexBuffer;
}

const std::uint32_t& shade::OpenGLVertexArray::GetRenderID() const
{
	return m_RenderID;
}
