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

		virtual void Bind(std::uint32_t binding = 0) const override;
		virtual std::uint32_t GetRenderID() override;
		virtual void AssetInit() override;
		static void BindTest(std::uint32_t renderId, std::uint32_t binding);
	private:
		GLuint  m_RenderID;
		bool    m_IsInit;
		void	_Init();
	};
}