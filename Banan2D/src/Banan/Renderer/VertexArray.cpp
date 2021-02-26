#include "bgepch.h"
#include "VertexArray.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Banan
{

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BANAN_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
		}

		BANAN_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}