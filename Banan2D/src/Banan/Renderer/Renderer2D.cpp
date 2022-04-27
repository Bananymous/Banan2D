#include "bgepch.h"
#include "Renderer2D.h"

#include "./VertexArray.h"
#include "./Shader.h"
#include "./RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

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

	struct CircleVertex
	{
		glm::vec3 worldPosition;
		glm::vec3 localPosition;
		glm::vec4 color;
		float thickness;
		float fade;
	};

	struct LineVertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	static struct Renderer2DData
	{
		static const uint32_t maxQuads			= 25000;
		static const uint32_t maxVertices		= maxQuads * 4;
		static const uint32_t maxIndices		= maxQuads * 6;
		uint32_t maxTextureSlots				= 0;

		Ref<VertexArray>	quadVertexArray;
		Ref<VertexBuffer>	quadVertexBuffer;
		Ref<Texture2D>		whiteTexture;
		Ref<Shader>			quadShader;

		Ref<VertexArray>	circleVertexArray;
		Ref<VertexBuffer>	circleVertexBuffer;
		Ref<Shader>			circleShader;

		Ref<VertexArray>	lineVertexArray;
		Ref<VertexBuffer>	lineVertexBuffer;
		Ref<Shader>			lineShader;

		uint32_t		quadIndexCount			= 0;
		QuadVertex*		quadVertexBufferBase	= nullptr;
		QuadVertex*		quadVertexBufferPtr		= nullptr;

		uint32_t		circleIndexCount		= 0;
		CircleVertex*	circleVertexBufferBase	= nullptr;
		CircleVertex*	circleVertexBufferPtr	= nullptr;

		uint32_t		lineVertexCount			= 0;
		LineVertex*		lineVertexBufferBase	= nullptr;
		LineVertex*		lineVertexBufferPtr		= nullptr;

		std::vector<Ref<Texture2D>> textureSlots;
		uint32_t textureSlotIndex = 1;

		glm::vec4 quadVertexPositions[4]{};

		Renderer2D::Stats stats;
		
	} s_data;

	void Renderer2D::Init()
	{
		RenderCommand::Init();

		s_data.maxTextureSlots = RenderCommand::GetMaxTextureSlots();

		// --------- Quads ---------
		{
			s_data.quadVertexArray = VertexArray::Create();

			s_data.quadVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));
			s_data.quadVertexBuffer->SetLayout(BufferLayout({
				{ ShaderDataType::Float3, "a_position"     },
				{ ShaderDataType::Float4, "a_color"        },
				{ ShaderDataType::Float2, "a_textureCoord" },
				{ ShaderDataType::Float,  "a_textureIndex" },
				{ ShaderDataType::Float,  "a_tilingFactor" }
			}));
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
		}

		// -------- Circles --------
		{
			s_data.circleVertexArray = VertexArray::Create();

			s_data.circleVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));
			s_data.circleVertexBuffer->SetLayout(BufferLayout({
				{ ShaderDataType::Float3, "a_worldPosition" },
				{ ShaderDataType::Float3, "a_localPosition" },
				{ ShaderDataType::Float4, "a_color"			},
				{ ShaderDataType::Float,  "a_thickness"		},
				{ ShaderDataType::Float,  "a_fade"			}
			}));
			s_data.circleVertexArray->AddVertexBuffer(s_data.circleVertexBuffer);
			s_data.circleVertexArray->SetIndexBuffer(s_data.quadVertexArray->GetIndexBuffer()); // Use quad IB
			s_data.circleVertexBufferBase = new CircleVertex[s_data.maxVertices];
		}

		// --------- Lines ---------
		{
			s_data.lineVertexArray = VertexArray::Create();

			s_data.lineVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(LineVertex));
			s_data.lineVertexBuffer->SetLayout(BufferLayout({
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float4, "a_color"    }
			}));
			s_data.lineVertexArray->AddVertexBuffer(s_data.lineVertexBuffer);
			s_data.lineVertexBufferBase = new LineVertex[s_data.maxVertices];
		}

		s_data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_data.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Shaders
		{
			int* samplers = new int[s_data.maxTextureSlots];
			for (uint32_t i = 0; i < s_data.maxTextureSlots; i++)
				samplers[i] = i;
			s_data.quadShader = Shader::CreateTextureShader(s_data.maxTextureSlots);
			s_data.quadShader->Bind();
			s_data.quadShader->SetIntArray("u_textures", samplers, s_data.maxTextureSlots);
			delete[] samplers;

			s_data.circleShader = Shader::CreateCircleShader();

			s_data.lineShader = Shader::CreateLineShader();
		}

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
		s_data.quadShader->Bind();
		s_data.quadShader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());
	
		s_data.circleShader->Bind();
		s_data.circleShader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());

		s_data.lineShader->Bind();
		s_data.lineShader->SetMat4("u_viewProjection", camera.GetViewProjectionMatrix());

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

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;

		s_data.lineVertexCount = 0;
		s_data.lineVertexBufferPtr = s_data.lineVertexBufferBase;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_data.quadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase);
			s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
				s_data.textureSlots[i]->Bind(i);

			s_data.quadShader->Bind();
			RenderCommand::DrawIndexed(s_data.quadVertexArray, s_data.quadIndexCount);

			s_data.stats.drawCalls++;
		}

		if (s_data.circleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.circleVertexBufferPtr - (uint8_t*)s_data.circleVertexBufferBase);
			s_data.circleVertexBuffer->SetData(s_data.circleVertexBufferBase, dataSize);

			s_data.circleShader->Bind();
			RenderCommand::DrawIndexed(s_data.circleVertexArray, s_data.circleIndexCount);

			s_data.stats.drawCalls++;
		}

		if (s_data.lineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.lineVertexBufferPtr - (uint8_t*)s_data.lineVertexBufferBase);
			s_data.lineVertexBuffer->SetData(s_data.lineVertexBufferBase, dataSize);

			s_data.lineShader->Bind();
			RenderCommand::DrawLines(s_data.lineVertexArray, s_data.lineVertexCount);

			s_data.stats.drawCalls++;
		}
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

		DrawQuad(transform, nullptr, glm::vec2(0.0f), glm::vec2(1.0f), 1.0f, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, texture, glm::vec2(0.0f), glm::vec2(1.0f), 1.0f, glm::vec4(1.0f));
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, nullptr, glm::vec2(0.0f), glm::vec2(1.0f), 1.0f, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawQuad(transform, texture, glm::vec2(0.0f), glm::vec2(1.0f), 1.0f, glm::vec4(1.0f));
	}


	void Renderer2D::DrawQuad(const QuadProperties& props)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(props.position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		DrawQuad(transform, props.texture, props.min, props.max, props.tilingFactor, props.color);
	}

	void Renderer2D::DrawRotatedQuad(const QuadProperties& props)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(props.position, 0.0f))
			* glm::rotate(glm::mat4(1.0f), props.rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(props.size, 1.0f));

		DrawQuad(transform, props.texture, props.min, props.max, props.tilingFactor, props.color);
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

			s_data.stats.textures++;
		}

		return textureIndex;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max, float tilingFactor, const glm::vec4& color)
	{
		if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
			NextBatch();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = texture ? GetTextureIndex(texture) : 0.0f;
		const glm::vec2 textureCoords[] = { { min.x, min.y }, { max.x, min.y }, { max.x, max.y }, { min.x, max.y } };

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

		s_data.stats.quads++;
	}


	void Renderer2D::DrawCircle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float thickness, float fade)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		DrawCircle(transform, color, thickness, fade);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade)
	{
		// TODO: implement for circles
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_data.circleVertexBufferPtr->worldPosition = transform * s_data.quadVertexPositions[i];
			s_data.circleVertexBufferPtr->localPosition = s_data.quadVertexPositions[i] * 2.0f;
			s_data.circleVertexBufferPtr->color = color;
			s_data.circleVertexBufferPtr->thickness = thickness;
			s_data.circleVertexBufferPtr->fade = fade;
			s_data.circleVertexBufferPtr++;
		}

		s_data.circleIndexCount += 6;

		s_data.stats.quads++;
	}

	void Renderer2D::DrawLine(const glm::vec2& from, const glm::vec2& to, const glm::vec4& color)
	{
		s_data.lineVertexBufferPtr->position = glm::vec3(from, 0.0f);
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr++;

		s_data.lineVertexBufferPtr->position = glm::vec3(to, 0.0f);
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr++;

		s_data.lineVertexCount += 2;
	}

	void Renderer2D::Draw_Text(const std::string& text, const glm::vec2& position, float size, const glm::vec4& color, const Ref<Font>& font)
	{
		if (text.empty())
			return;

		glm::vec2 cursor = position;

		const auto& char_data = font->GetCharacters();
		const auto& kerning_data = font->GetKernings();

		QuadProperties char_quad;
		char_quad.tint = color;
		char_quad.texture = font->GetTexture();

		for (uint32_t i = 0; i < text.size(); i++)
		{
			if (char_data.find(text[i]) != char_data.end())
			{
				const auto& current_data = char_data.at(text[i]);

				float kerning = 0.0f;
				if (i > 0 && kerning_data.find(text[i - 1]) != kerning_data.end())
				{
					const auto& kerning_map = kerning_data.at(text[i - 1]);
					if (kerning_map.find(text[i]) != kerning_map.end())
						kerning = kerning_map.at(text[i]);
				}

				char_quad.position = cursor;

				char_quad.position.x += (current_data.offset.x + current_data.size.x * 0.5f + kerning) * size;
				char_quad.position.y -= (current_data.offset.y + current_data.size.y * 0.5f) * size;

				char_quad.size = current_data.size * size;

				char_quad.min = current_data.min;
				char_quad.max = current_data.max;

				DrawQuad(char_quad);

				cursor.x += (current_data.advance + kerning) * size;
			}
		}
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