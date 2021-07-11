#include "shade_pch.h"
#include "VertexArray.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/buffers/OpenGLVertexArray.h"


void shade::VertexArray::RemoveVertexBuffer(const std::uint32_t& renderId)
{
	auto buffer = std::find_if(std::begin(m_VertexBuffers), std::end(m_VertexBuffers), [&](const Shared<VertexBuffer>& buffer)
		{
			return buffer->GetRenderID() == renderId;
		});

	if (buffer != m_VertexBuffers.end())
		m_VertexBuffers.erase(buffer);
}

shade::Shared<shade::VertexArray> shade::VertexArray::Create()
{
	switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL:	return CreateShared<OpenGLVertexArray>();
	case RenderAPI::API::Vulkan:
		SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

const std::vector<shade::Shared<shade::VertexBuffer>>& shade::VertexArray::GetVertexBuffers() const
{
	return m_VertexBuffers;
}

std::vector<shade::Shared<shade::VertexBuffer>>& shade::VertexArray::GetVertexBuffers()
{
	return const_cast<std::vector<Shared<VertexBuffer>>&>(const_cast<const VertexArray*>(this)->GetVertexBuffers());
}

const shade::Shared<shade::IndexBuffer>& shade::VertexArray::GetIndexBuffer() const
{
	return m_IndexBuffer;
}

shade::Shared<shade::IndexBuffer>& shade::VertexArray::GetIndexBuffer()
{
	return const_cast<Shared<IndexBuffer>&>(const_cast<const VertexArray*>(this)->GetIndexBuffer());
}
