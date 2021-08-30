#pragma once
#include "shade/core/render/buffers/ShaderStorageBuffer.h"

namespace shade
{
	class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(const std::uint32_t& size, const std::uint32_t& binding);
		virtual ~OpenGLShaderStorageBuffer();

		virtual void SetData(const void* data, const std::uint32_t& size, const std::uint32_t& offset = 0) override;
		virtual void Resize(const std::uint32_t& size) override;
		virtual std::uint32_t GetSize() const override;
	private:
		std::uint32_t m_RenderID = 0;
		std::uint32_t m_Binding = 0;
	};
}