#pragma once

#include "../../Banan/Renderer/RenderContext.h"

namespace Banan
{

	class OpenGLContext : public RenderContext
	{
	public:
		virtual ~OpenGLContext() = default;

		static Scope<RenderContext> Create(Window* window);
	};

}