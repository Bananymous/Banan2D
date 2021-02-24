#pragma once

#include "BGE/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

typedef unsigned int GLenum;

namespace Banan
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& path);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_name; }

		void UploadUniformInt (const std::string& name, int value);
		void UploadUniformInt2(const std::string& name, const glm::ivec2& values);
		void UploadUniformInt3(const std::string& name, const glm::ivec3& values);
		void UploadUniformInt4(const std::string& name, const glm::ivec4& values);

		void UploadUniformFloat (const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& path);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_rendererID;
		std::string m_name;

	};

}