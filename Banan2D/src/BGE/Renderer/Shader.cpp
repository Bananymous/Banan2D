#include "bgepch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Banan
{

	Ref<Shader> Shader::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BGE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(path);
		}

		BGE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BGE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		BGE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		BGE_ASSERT(!Exists(name), "Shader already exists");
		m_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		Add(shader->GetName(), shader);
	}

	Banan::Ref<Banan::Shader> ShaderLibrary::Load(const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	Banan::Ref<Banan::Shader> ShaderLibrary::Load(const std::string& name, const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	Banan::Ref<Banan::Shader> ShaderLibrary::Get(const std::string& name)
	{
		BGE_ASSERT(Exists(name), "Shader not found");
		return m_shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_shaders.find(name) != m_shaders.end();
	}

}