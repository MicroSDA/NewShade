#pragma once
#include "shade/config/API.h"
#include "shade/core/utils/Log.h"
#include "shade/core/vertex/Vertex3D.h"

namespace shade
{
	class SHADE_API VertexBuffer
	{
	public:
		enum class ElementType
		{
			None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
		};
		enum class BufferType
		{
			Static	= 0x88E4, // OpenGL native values
			Dynamic = 0x88E8
		};
		class SHADE_API BufferElement
		{
		public:
			BufferElement(const ElementType& type, const std::string& name);
		public:
			uint32_t GetTypeCount() const;
			size_t	 GetTypeSize()	const;
		public:
			std::string				Name = "Undefined";
			ElementType				Type = ElementType::None;
			size_t                  Size = 0;
			size_t                  Offset = 0;
			bool                    Normalized = false;
			uint32_t                Id = 0;
		};
		class SHADE_API Layout
		{
		public:
			Layout();
			Layout(std::initializer_list<BufferElement> elements);
			size_t GetStride() const;

			std::vector<BufferElement>::iterator		begin();
			std::vector<BufferElement>::iterator		end();
			std::vector<BufferElement>::const_iterator	begin() const;
			std::vector<BufferElement>::const_iterator	end()	const;
			std::vector<BufferElement>::size_type		size()	const;

			const std::vector<BufferElement>& GetElements() const;
		private:
			std::vector<BufferElement>	m_Elements;
			size_t						m_Stride;

		};

	public:
		// Vertex Buffer
		virtual ~VertexBuffer()		= default;
		virtual void Bind()			= 0;
		virtual void UnBind() const	= 0;
		virtual void SetData(const uint32_t& offset, const uint32_t& size, const Vertex3D* data) = 0;
		virtual void SetLayout(const Layout& layout)		= 0;
		virtual const Layout& GetLayout()			const	= 0;

		static Shared<VertexBuffer> Create(const Vertex3D* data, const uint32_t& size, const BufferType& type);
		static Shared<VertexBuffer> Create(const uint32_t& size, const BufferType& type);
	private:

	};
}