#pragma once

#include "BGE/Renderer/Texture2D.h"

#include <glad/glad.h>

namespace Banan
{

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual void SetData(void* data, uint32_t size) override;

		virtual uint32_t GetWidth() const override	{ return m_width; }
		virtual uint32_t GetHeight() const override	{ return m_height; }

		virtual void Bind(uint32_t slot = 0) const override;

	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};

}