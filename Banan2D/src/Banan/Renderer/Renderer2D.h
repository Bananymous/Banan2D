#pragma once

#include "./Camera.h"
#include "./Texture2D.h"
#include "./Font.h"

namespace Banan
{

	struct QuadProperties
	{
		glm::vec2 position	= glm::vec2(0.0f);
		glm::vec2 size		= glm::vec2(1.0f);
		float rotation		= 0.0f;
		float tilingFactor	= 1.0f;
		union
		{
			glm::vec4 tint, color = glm::vec4(1.0f);
		};

		glm::vec2 min = glm::vec2(0.0f);
		glm::vec2 max = glm::vec2(1.0f);
		Ref<Texture2D> texture = nullptr;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);

		static void DrawQuad(const QuadProperties& properties);
		static void DrawRotatedQuad(const QuadProperties& properties);

		static void DrawCircle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);

		static void DrawLine(const glm::vec2& from, const glm::vec2& to, const glm::vec4& color);

		static void Draw_Text(const std::string& text, const glm::vec2& position, float size, const glm::vec4& color, const Ref<Font>& font);

		struct Stats
		{
			uint32_t drawCalls	= 0;
			uint32_t textures	= 0;
			uint32_t quads		= 0;
			uint32_t GetVertices()	const { return quads * 4; }
			uint32_t GetIndices()	const { return quads * 6; }
		};
		static Stats GetStats();
		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();

		static float GetTextureIndex(const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max, float tilingFactor, const glm::vec4& tint);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade);
	};

}