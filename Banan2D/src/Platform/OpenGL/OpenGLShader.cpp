#include "bgepch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <fstream>

namespace Banan
{

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_rendererID);
	}

	Ref<OpenGLShader> OpenGLShader::CreateTextureShader(uint32_t slotCount)
	{
		std::string vertexSource = R"(
			#version 450 core

			layout(location = 0) in vec3	a_position;
			layout(location = 1) in vec4	a_color;
			layout(location = 2) in vec2	a_textureCoord;
			layout(location = 3) in float	a_textureIndex;
			layout(location = 4) in float	a_tilingFactor;

			uniform mat4 u_viewProjection;

			struct VertexOutput
			{
				vec4 color;
				vec2 textureCoord;
				float tilingFactor;
			};

			layout(location = 0) out VertexOutput Output;
			layout(location = 3) out flat float v_textureIndex;

			void main()
			{
				Output.color = a_color;
				Output.textureCoord = a_textureCoord;
				Output.tilingFactor = a_tilingFactor;
				v_textureIndex = a_textureIndex;

				gl_Position = u_viewProjection * vec4(a_position, 1.0);
			}
		)";

		std::stringstream fragmentSource;

		fragmentSource << R"(
			#version 450 core

			layout(location = 0) out vec4 o_color;
			
			struct VertexOutput
			{
				vec4 color;
				vec2 textureCoord;
				float tilingFactor;
			};

			layout(location = 0) in VertexOutput Input;
			layout(location = 3) in flat float v_textureIndex;
			
			uniform sampler2D u_textures[32];
			
			void main()
			{
				vec4 textureColor = Input.color;
				switch(int(v_textureIndex))
				{
		)";

		for (uint32_t i = 0; i < slotCount; i++)
			fragmentSource << "case " << i << ": textureColor *= texture2D(u_textures[" << i << "],  Input.textureCoord * Input.tilingFactor); break;\r\n";

		fragmentSource << R"(
				}
				o_color = textureColor;
			}
		)";

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource.str();


		Ref<OpenGLShader> shader = CreateRef<OpenGLShader>();
		shader->Compile(shaderSources);
		return shader;
	}

	Ref<OpenGLShader> OpenGLShader::CreateCircleShader()
	{
		std::string vertexSource = R"(
			#version 450 core

			layout(location = 0) in vec3	a_worldPosition;
			layout(location = 1) in vec3	a_localPosition;
			layout(location = 2) in vec4	a_color;
			layout(location = 3) in float	a_thickness;
			layout(location = 4) in float	a_fade;

			uniform mat4 u_viewProjection;

			struct VertexOutput
			{
				vec3 localPosition;
				vec4 color;
				float thickness;
				float fade;
			};

			layout(location = 0) out VertexOutput Output;

			void main()
			{
				Output.localPosition = a_localPosition;
				Output.color = a_color;
				Output.thickness = a_thickness;
				Output.fade = a_fade;

				gl_Position = u_viewProjection * vec4(a_worldPosition, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 450 core

			layout(location = 0) out vec4 o_color;

			struct VertexOutput
			{
				vec3 localPosition;
				vec4 color;
				float thickness;
				float fade;
			};

			layout(location = 0) in VertexOutput Input;

			void main()
			{
				float distance = 1.0 - length(Input.localPosition);
				float circle = smoothstep(0.0, Input.fade, distance);
				circle *= smoothstep(Input.thickness + Input.fade, Input.thickness, distance);

				if (circle == 0.0)
					discard;

				o_color = Input.color;
				o_color.a *= circle;
			}
		)";

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

		Ref<OpenGLShader> shader = CreateRef<OpenGLShader>();
		shader->Compile(shaderSources);
		return shader;
	}

	Ref<OpenGLShader> OpenGLShader::CreateLineShader()
	{
		std::string vertexSource = R"(
			#version 450 core

			layout(location = 0) in vec3	a_position;
			layout(location = 2) in vec4	a_color;

			uniform mat4 u_viewProjection;

			struct VertexOutput
			{
				vec4 color;
			};

			layout(location = 0) out VertexOutput Output;

			void main()
			{
				Output.color = a_color;

				gl_Position = u_viewProjection * vec4(a_position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 450 core

			layout(location = 0) out vec4 o_color;

			struct VertexOutput
			{
				vec4 color;
			};

			layout(location = 0) in VertexOutput Input;

			void main()
			{
				o_color = Input.color;
			}
		)";

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

		Ref<OpenGLShader> shader = CreateRef<OpenGLShader>();
		shader->Compile(shaderSources);
		return shader;
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		BANAN_ASSERT(shaderSources.size() <= 4, "You can use 2 shaders at most");

		std::array<GLuint, 2> glShaderIDs{};

		int glShaderIDIndex = 0;
		for (auto& key : shaderSources)
		{
			GLenum type = key.first;
			const std::string& source = key.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (compiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				BANAN_ERROR("Failed to compile %i shader!\n", type);
				BANAN_ASSERT(false, infoLog.data());
				
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto ID : glShaderIDs)
				glDeleteShader(ID);

			BANAN_PRINT("Failed to link shaders!\n");
			BANAN_ASSERT(false, infoLog.data());
			
			return;
		}
		
		for (auto ID : glShaderIDs)
			glDetachShader(program, ID);

		m_rendererID = program;
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformInt2(const std::string& name, const glm::ivec2& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2i(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformInt3(const std::string& name, const glm::ivec3& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3i(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformInt4(const std::string& name, const glm::ivec4& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4i(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, values);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}