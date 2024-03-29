#pragma once

#include "Banan/Core/Base.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

namespace Banan
{
	
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static Ref<Shader> CreateTextureShader(uint32_t textureSlots);
		static Ref<Shader> CreateCircleShader();
		static Ref<Shader> CreateLineShader();
	};

}