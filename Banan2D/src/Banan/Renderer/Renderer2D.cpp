#include "bgepch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include <bitset>

namespace Banan
{

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
		float textureIndex;
		float tilingFactor;
	};

	static struct Renderer2DData
	{
		static const uint32_t maxQuads			= 25000;
		static const uint32_t maxVertices		= maxQuads * 4;
		static const uint32_t maxIndices		= maxQuads * 6;
		uint32_t maxTextureSlots;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Texture2D> whiteTexture;
		Ref<Shader> shader;

		uint32_t quadIndexCount				= 0;
		QuadVertex* quadVertexBufferBase	= nullptr;
		QuadVertex* quadVertexBufferPtr		= nullptr;

		std::vector<Ref<Texture2D>> textureSlots;
		uint32_t textureSlotIndex = 1;

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Stats stats;
		
	} s_data;

	void Renderer2D::Init()
	{
		RenderCommand::Init();

		s_data.maxTextureSlots = RenderCommand::GetMaxTextureSlots();

		s_data.quadVertexArray = VertexArray::Create();

		s_data.quadVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_position"     },
			{ ShaderDataType::Float4, "a_color"        },
			{ ShaderDataType::Float2, "a_textureCoord" },
			{ ShaderDataType::Float,  "a_textureIndex" },
			{ ShaderDataType::Float,  "a_tilingFactor" }
		});
		s_data.quadVertexArray->AddVertexBuffer(s_data.quadVertexBuffer);

		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_data.maxIndices];

		for (uint32_t i = 0, offset = 0; i < s_data.maxIndices; i += 6, offset += 4)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		}

		auto quadIB = IndexBuffer::Create(quadIndices, s_data.maxIndices);
		s_data.quadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_data.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int* samplers = new int[s_data.maxTextureSlots];
		for (uint32_t i = 0; i < s_data.maxTextureSlots; i++)
			samplers[i] = i;
		s_data.shader = Shader::Create(s_data.maxTextureSlots);
		s_data.shader->Bind();
		s_data.shader->SetIntArray("u_textures", samplers, s_data.maxTextureSlots);
		delete[] samplers;

		s_data.textureSlots.resize(s_data.maxTextureSlots);
		s_data.textureSlots[0] = s_data.whiteTexture;

		s_data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_data.quadVertexBufferBase;
	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());
	
		StartBatch();
	}
	 
	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_data.quadIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase);
		s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
			s_data.textureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_data.quadVertexArray, s_data.quadIndexCount);

		s_data.stats.drawCalls++;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, nullptr, 1.0f, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, texture, 1.0f, glm::vec4(1.0f));
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, nullptr, 1.0f, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, texture, 1.0f, glm::vec4(1.0f));
	}


	void Renderer2D::DrawQuad(const QuadProperties& props)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(props.position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		DrawQuad(transform, props.texture, props.tilingFactor, props.color);
	}

	void Renderer2D::DrawRotatedQuad(const QuadProperties& props)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(props.position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), props.rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		DrawQuad(transform, props.texture, props.tilingFactor, props.color);
	}

	float Renderer2D::GetTextureIndex(const Ref<Texture2D>& texture)
	{
		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
		{
			if (*s_data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_data.textureSlotIndex >= s_data.maxTextureSlots)
				NextBatch();

			textureIndex = (float)s_data.textureSlotIndex;
			s_data.textureSlots[s_data.textureSlotIndex] = texture;
			s_data.textureSlotIndex++;
		}

		return textureIndex;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& color)
	{
		if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
			NextBatch();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = texture ? GetTextureIndex(texture) : 0.0f;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_data.quadVertexBufferPtr->position		= transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color			= color;
			s_data.quadVertexBufferPtr->textureCoord	= textureCoords[i];
			s_data.quadVertexBufferPtr->textureIndex	= textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor	= tilingFactor;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		if (textureIndex > s_data.stats.textures)
			s_data.stats.textures = (uint32_t)textureIndex;
		s_data.stats.quads++;
	}


	Renderer2D::Stats Renderer2D::GetStats()
	{
		return s_data.stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_data.stats, 0, sizeof(Stats));
	}

}