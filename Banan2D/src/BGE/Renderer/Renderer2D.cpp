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
	};

	struct Renderer2DData
	{
		const uint32_t maxQuads		= 10000;
		const uint32_t maxVertices	= maxQuads * 4;
		const uint32_t maxIndices	= maxQuads * 6;
		static const uint32_t maxTextureSlots = 32;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Texture2D> whiteTexture;
		Ref<Shader> shader;

		uint32_t quadIndexCount				= 0;
		QuadVertex* quadVertexBufferBase	= nullptr;
		QuadVertex* quadVertexBufferPtr		= nullptr;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;

		glm::vec4 quadVertexPositions[4];
	};

	static Renderer2DData s_data;



	void Renderer2D::Init()
	{
		RenderCommand::Init();

		s_data.quadVertexArray = VertexArray::Create();


		s_data.quadVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_position"     },
			{ ShaderDataType::Float4, "a_color"        },
			{ ShaderDataType::Float2, "a_textureCoord" },
			{ ShaderDataType::Float,  "a_textureIndex" }
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

		int samplers[s_data.maxTextureSlots];
		for (uint32_t i = 0; i < s_data.maxTextureSlots; i++)
			samplers[i] = i;

		s_data.shader = Shader::Create("assets/shaders/Shader2D.glsl");
		s_data.shader->Bind();
		s_data.shader->SetIntArray("u_textures", samplers, s_data.maxTextureSlots);

		s_data.textureSlots[0] = s_data.whiteTexture;

		s_data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{

	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());
	
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.textureSlotIndex = 1;
	}
	 
	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase);
		s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
			s_data.textureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_data.quadVertexArray, s_data.quadIndexCount);
	}


	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.color = color;
		DrawQuad(props);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.texture = texture;
		DrawQuad(props);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.rotation = rotation;
		props.color = color;
		DrawRotatedQuad(props);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.rotation = rotation;
		props.texture = texture;
		DrawRotatedQuad(props);
	}

	static float GetTextureIndex(const Ref<Texture2D>& texture)
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
			textureIndex = (float)s_data.textureSlotIndex;
			s_data.textureSlots[s_data.textureSlotIndex] = texture;
			s_data.textureSlotIndex++;
		}

		return textureIndex;
	}

	void Renderer2D::DrawQuad(const QuadPropreties& props)
	{
		float textureIndex = props.texture ? GetTextureIndex(props.texture) : 0.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), props.position)
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		for (int i = 0; i < 4; i++)
		{
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = props.color;
			s_data.quadVertexBufferPtr->textureCoord = { (float)((i >> 1) ^ (i & 1)), (float)(i >> 1) };
			s_data.quadVertexBufferPtr->textureIndex = textureIndex;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;
	}

	void Renderer2D::DrawRotatedQuad(const QuadPropreties& props)
	{
		float textureIndex = props.texture ? GetTextureIndex(props.texture) : 0.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), props.position)
			* glm::rotate(glm::mat4(1.0f), props.rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		for (int i = 0; i < 4; i++)
		{
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = props.color;
			s_data.quadVertexBufferPtr->textureCoord = { (float)((i >> 1) ^ (i & 1)), (float)(i >> 1) };
			s_data.quadVertexBufferPtr->textureIndex = textureIndex;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;
	}

}