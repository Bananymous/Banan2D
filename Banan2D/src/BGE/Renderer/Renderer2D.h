#pragma once

#include "Camera.h"
#include "Texture2D.h"

namespace Banan
{

	struct QuadPropreties {
		glm::vec3 position;
		glm::vec2 size;
		float rotation;
		union { glm::vec4 tint, color; };
		Ref<Texture2D> texture;

		QuadPropreties() :
			position	(glm::vec3(0.0f)),
			size		(glm::vec2(1.0f)),
			rotation	(0.0f),
			color		(glm::vec4(1.0f)),
			texture		(nullptr)
		{ }
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);

		static void DrawQuad(const QuadPropreties& properties);
		static void DrawRotatedQuad(const QuadPropreties& properties);

	};

}