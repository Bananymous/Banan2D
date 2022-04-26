#pragma once

#include "Banan/Core/ConsoleOutput.h"

#include "Banan/Core/Base.h"

#include <string>
#include <vector>
#include <initializer_list>

#include <cstdint>
#include <cstring>

namespace Banan
{

	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Banan::ShaderDataType::Float:	return 4;
			case Banan::ShaderDataType::Float2:	return 4 * 2;
			case Banan::ShaderDataType::Float3:	return 4 * 3;
			case Banan::ShaderDataType::Float4:	return 4 * 4;
			case Banan::ShaderDataType::Mat3:	return 4 * 3 * 3;
			case Banan::ShaderDataType::Mat4:	return 4 * 4 * 4;
			case Banan::ShaderDataType::Int:	return 4;
			case Banan::ShaderDataType::Int2:	return 4 * 2;
			case Banan::ShaderDataType::Int3:	return 3 * 3;
			case Banan::ShaderDataType::Int4:	return 4 * 4;
			case Banan::ShaderDataType::Bool:	return 1;
		}

		BANAN_ASSERT(false, "Unknown data type!");
		return 0;
	}

	struct BufferElement
	{		
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) :
			name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
		{ }

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
				case Banan::ShaderDataType::Float:	return 1;
				case Banan::ShaderDataType::Float2:	return 2;
				case Banan::ShaderDataType::Float3:	return 3;
				case Banan::ShaderDataType::Float4:	return 4;
				case Banan::ShaderDataType::Mat3:	return 3;
				case Banan::ShaderDataType::Mat4:	return 4;
				case Banan::ShaderDataType::Int:	return 1;
				case Banan::ShaderDataType::Int2:	return 2;
				case Banan::ShaderDataType::Int3:	return 3;
				case Banan::ShaderDataType::Int4:	return 4;
				case Banan::ShaderDataType::Bool:	return 1;
			}

			BANAN_ASSERT(false, "Unknown data type!");
			return 0;
		}

	public:
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

	};

	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& elements = {}) :
			m_elements(elements), m_stride(0)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const								{ return m_stride; }
		const std::vector<BufferElement>& GetElements() const	{ return m_elements; }

		std::vector<BufferElement>::iterator begin()				{ return m_elements.begin(); }
		std::vector<BufferElement>::iterator end()					{ return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin()	const	{ return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end()	const	{ return m_elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_stride = 0;
			for (BufferElement& element : m_elements)
			{
				element.offset = offset;
				offset += element.size;
				m_stride += element.size;
			}
		}

	private:
		std::vector<BufferElement> m_elements;
		uint32_t m_stride;

	};


	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
	
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
	
		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);

	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	};

}