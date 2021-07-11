#pragma once
#include "shade/core/image/Texture.h"
#include <glad/glad.h>
#include <glad/glad.h>

namespace shade
{
	class OpenGLTexture :public Texture
	{
	public:
		OpenGLTexture();
		virtual ~OpenGLTexture();

		virtual void Bind(const Shared<Shader>& shader, std::uint32_t binding = 0) const override;
		virtual std::uint32_t GetRenderID() override;
		virtual void AssetInit() override;
	private:
		GLuint  m_RenderID;
		bool    m_IsInit;
		void	_Init();
	};
}