#pragma once
#include "shade/config/API.h"
#include "shade/core/Types.h"
#include "shade/core/utils/Log.h"

namespace shade
{
	class SHADE_API FrameBuffer
	{
	public:
		struct Texture
		{
			enum class Format
			{
				None = 0,
				RGBA8,
				RGBA10,
				RGBA16F,
				RED_INT,
				DEPTH24STENCIL8
			};
			struct Data
			{
				Data():
					R(-1), G(-1), B(-1)
				{}
				Data(const std::int32_t& r, const std::int32_t& g, const std::int32_t& b):
					R(r), G(g), B(b) 
				{}
				std::int32_t R, G, B;
			};
			Texture() = default;
			Texture(Format format):
				TextureFormat(format)
			{}
			Texture::Format TextureFormat;
		};
		struct Attachment
		{
			Attachment() = default;
			Attachment(std::initializer_list<Texture> attachments):
				Attachments(attachments)
			{}

			std::vector<Texture> Attachments;
		};
		struct Layout
		{
			Layout(const std::uint32_t& width, const std::uint32_t& height, const Attachment& attachments, const bool& swapChainTarget = false):
				Width(width), Height(height),Attachments(attachments), IsSwapChainTarget(swapChainTarget)
			{}
			std::uint32_t Width = 0, Height = 0;
			Attachment Attachments;
			std::uint32_t Samples		= 1;
			bool IsSwapChainTarget		= false;
		};
	public:
		virtual ~FrameBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void Resize(const std::uint32_t& width, const std::uint32_t& height) = 0;
		virtual Texture::Data GetData(const std::uint32_t& attachment, const std::uint32_t& x, const std::uint32_t& y) const = 0;
		virtual std::uint32_t GetAttachment(const std::uint32_t& index) const = 0;
		virtual void BindTextureAttachment(const std::uint32_t& index, const std::uint32_t& unit) const = 0;
		template<typename T>
		void ClearAttachment(const std::uint32_t& index, const T& clearValue);
		virtual const Layout& GetLayout() const = 0;

		static Shared<FrameBuffer> Create(const Layout& layout);
	protected:
		virtual void _ClearAttachmentInt(const std::uint32_t& attachment,   const std::int32_t& clearValue = 0) = 0;
		virtual void _ClearAttachmentFloat(const std::uint32_t& attachment, const float& clearValue = 0) = 0;
	};

	template<typename T>
	inline void FrameBuffer::ClearAttachment(const std::uint32_t& index, const T& clearValue)
	{
		if (std::is_same<T, std::int32_t>::value)
			_ClearAttachmentInt(index, clearValue);
		else if (std::is_same<T, float>::value)
			_ClearAttachmentFloat(index, clearValue);
		else
			SHADE_CORE_WARNING("Unsupported frame buffer clear type!");
	}
}