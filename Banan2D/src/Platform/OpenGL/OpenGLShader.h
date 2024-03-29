#pragma once

#include "Banan/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

typedef unsigned int GLenum;

namespace Banan
{

	class OpenGLShader : public Shader
	{
	public:
		virtual ~OpenGLShader();

		static Ref<OpenGLShader> CreateTextureShader(uint32_t slotCount);
		static Ref<OpenGLShader> CreateCircleShader();
		static Ref<OpenGLShader> CreateLineShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		void UploadUniformInt (const std::string& name, int value);
		void UploadUniformInt2(const std::string& name, const glm::ivec2& values);
		void UploadUniformInt3(const std::string& name, const glm::ivec3& values);
		void UploadUniformInt4(const std::string& name, const glm::ivec4& values);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat (const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_rendererID = 0;
	};

}