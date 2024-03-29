#include "bgepch.h"
#include "Shader.h"

#include "Banan/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Banan
{

	Ref<Shader> Shader::CreateTextureShader(uint32_t textureSlots)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BANAN_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return OpenGLShader::CreateTextureShader(textureSlots);
		}

		BANAN_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateCircleShader()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BANAN_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return OpenGLShader::CreateCircleShader();
		}

		BANAN_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateLineShader()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BANAN_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return OpenGLShader::CreateLineShader();
		}

		BANAN_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}