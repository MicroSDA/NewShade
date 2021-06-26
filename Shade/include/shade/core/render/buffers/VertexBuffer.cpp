#include "shade_pch.h"
#include "VertexBuffer.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLVertexBuffer.h"

shade::VertexBuffer::BufferElement::BufferElement(const ElementType& type, const std::string& name) :
    Type(type), Name(name), Size(GetTypeSize())
{
}

uint32_t shade::VertexBuffer::BufferElement::GetTypeCount() const
{
	switch (Type)
	{
	case ElementType::None:    return 0;
	case ElementType::Float:   return 1;
	case ElementType::Float2:  return 2;
	case ElementType::Float3:  return 3;
	case ElementType::Float4:  return 4;
	case ElementType::Mat3:    return 3;
	case ElementType::Mat4:    return 4;
	case ElementType::Int:     return 1;
	case ElementType::Int2:    return 2;
	case ElementType::Int3:    return 3;
	case ElementType::Int4:    return 4;
	case ElementType::Bool:    return 1;

	default: SHADE_CORE_WARNING("Undefined element type count in vertex buffer!"); return 0;
	}
}

size_t shade::VertexBuffer::BufferElement::GetTypeSize() const
{
	switch (Type)
	{
	case ElementType::None:		return 0;
	case ElementType::Float:	return 4;
	case ElementType::Float2:	return 8;
	case ElementType::Float3:	return 12;
	case ElementType::Float4:	return 16;
	case ElementType::Mat3:		return 32;
	case ElementType::Mat4:		return 64;
	case ElementType::Int:		return 4;
	case ElementType::Int2:		return 8;
	case ElementType::Int3:		return 12;
	case ElementType::Int4:		return 16;
	case ElementType::Bool:		return 1;

	default: return 0;
	}
}

shade::VertexBuffer::Layout::Layout() : 
	m_Stride(0)
{
}

shade::VertexBuffer::Layout::Layout(std::initializer_list<BufferElement> elements):
	m_Elements(elements), m_Stride(0)
{
	size_t   offset = 0;
	uint32_t id = 0;
	for (auto& element : m_Elements)
	{
		element.Offset	= offset;
		offset			+= element.Size;
		m_Stride		+= element.Size;
		element.Id		= id;
		id++;
	}
}

size_t shade::VertexBuffer::Layout::GetStride() const
{
	return m_Stride;
}

std::vector<shade::VertexBuffer::BufferElement>::iterator shade::VertexBuffer::Layout::begin()
{
	return m_Elements.begin();
}

std::vector<shade::VertexBuffer::BufferElement>::iterator shade::VertexBuffer::Layout::end()
{
	return m_Elements.end();
}

std::vector<shade::VertexBuffer::BufferElement>::const_iterator shade::VertexBuffer::Layout::begin() const
{
	return m_Elements.begin();
}

std::vector<shade::VertexBuffer::BufferElement>::const_iterator shade::VertexBuffer::Layout::end() const
{
	return m_Elements.end();
}

std::vector<shade::VertexBuffer::BufferElement>::size_type shade::VertexBuffer::Layout::size() const
{
	return m_Elements.size();
}

const std::vector<shade::VertexBuffer::BufferElement>& shade::VertexBuffer::Layout::GetElements() const
{
	return m_Elements;
}

shade::Shared<shade::VertexBuffer> shade::VertexBuffer::Create(const Vertex3D* data, const uint32_t& size, const BufferType& type)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLVertexBuffer>(data, size, type);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

shade::Shared<shade::VertexBuffer> shade::VertexBuffer::Create(const uint32_t& size, const BufferType& type)
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:  SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL: return CreateShared<OpenGLVertexBuffer>(size, type);
	case RenderAPI::API::Vulkan:SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default: SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}
