#include "bgepch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Banan
{

	Ref<Shader> Shader::Create(uint32_t textureSlots)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BGE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(textureSlots);
		}

		BGE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}