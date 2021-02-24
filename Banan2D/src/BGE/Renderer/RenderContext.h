#pragma once

#include "BGE/Core/Window.h"

namespace Banan
{
	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void SetVSync(bool enable) = 0;

		static Scope<RenderContext> Create(Window* window);

	};

}