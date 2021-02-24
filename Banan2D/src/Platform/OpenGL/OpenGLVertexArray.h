#pragma once

#include "BGE/Renderer/VertexArray.h"

#include "BGE/Core/Base.h"

namespace Banan
{

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffer() const override	{ return m_vertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override					{ return m_indexBuffer; }

	private:
		uint32_t m_rendererID;
		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;

	};

}