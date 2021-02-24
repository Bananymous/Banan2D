#include "bgepch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Banan
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Banan::ShaderDataType::Float:	return GL_FLOAT;
			case Banan::ShaderDataType::Float2:	return GL_FLOAT;
			case Banan::ShaderDataType::Float3:	return GL_FLOAT;
			case Banan::ShaderDataType::Float4:	return GL_FLOAT;
			case Banan::ShaderDataType::Mat3:	return GL_FLOAT;
			case Banan::ShaderDataType::Mat4:	return GL_FLOAT;
			case Banan::ShaderDataType::Int:	return GL_INT;
			case Banan::ShaderDataType::Int2:	return GL_INT;
			case Banan::ShaderDataType::Int3:	return GL_INT;
			case Banan::ShaderDataType::Int4:	return GL_INT;
			case Banan::ShaderDataType::Bool:	return GL_BOOL;
		}

		BGE_ASSERT(false, "Unknown data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		BGE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_rendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset
			);
			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_rendererID);
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}

}