#include "bgepch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

#pragma warning(push)
#pragma warning(disable : 4312)

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

		BANAN_ASSERT(false, "Unknown data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() :
		m_vertexBufferIndex(0)
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
		BANAN_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_rendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.type)
			{
				case ShaderDataType::Float: case ShaderDataType::Float2: case ShaderDataType::Float3: case ShaderDataType::Float4:
				case ShaderDataType::Int:   case ShaderDataType::Int2:   case ShaderDataType::Int3:   case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(m_vertexBufferIndex);
					glVertexAttribPointer(m_vertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.offset)
					);
					m_vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3: case ShaderDataType::Mat4:
				{
					uint32_t count = element.GetComponentCount();
					for (uint32_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(m_vertexBufferIndex);
						glVertexAttribPointer(m_vertexBufferIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.type),
							element.normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(sizeof(float) * count * i)
						);
						glVertexAttribDivisor(m_vertexBufferIndex, 1);
						m_vertexBufferIndex++;
					}
					break;
				}
				default: BANAN_ASSERT(false, "Unknown ShaderDataType!");
			}
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

#pragma warning(pop)