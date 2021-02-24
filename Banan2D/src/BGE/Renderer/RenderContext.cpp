#include "bgepch.h"
#include "RenderContext.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Banan
{

	Scope<RenderContext> RenderContext::Create(Window* window)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BGE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return OpenGLContext::Create(window);
		}

		BGE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}