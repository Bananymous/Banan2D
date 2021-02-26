#pragma once

#include "Banan/Renderer/Buffer.h"

namespace Banan
{

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& GetLayout() const override	{ return m_layout; }

		virtual void SetData(const void* data, uint32_t size) override;

	private:
		uint32_t m_rendererID;
		BufferLayout m_layout;

	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override	{ return m_count; }

	private:
		uint32_t m_rendererID;
		uint32_t m_count;

	};

}