#pragma once

#include "RendererAPI.h"

#include "Banan/Core/Base.h"

namespace Banan
{

	class RenderCommand
	{
	public:
		static void Init() { s_rendererAPI->Init(); }

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)		{ s_rendererAPI->SetViewport(x, y, width, height); }

		static void SetClearColor(const glm::vec4& color)										{ s_rendererAPI->SetClearColor(color); }
		static void Clear()																		{ s_rendererAPI->Clear(); }
		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)	{ s_rendererAPI->DrawIndexed(vertexArray, indexCount); }

		static uint32_t GetMaxTextureSlots()													{ return s_rendererAPI->GetMaxTextureSlots(); };

	private:
		static RendererAPI* s_rendererAPI;

	};

}