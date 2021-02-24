#include "bgepch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Banan
{

	static GLenum ShaderFromTypeString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
		BGE_ASSERT(false, "Invalid shader type: " << type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& path)
	{
		std::string source = ReadFile(path);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		size_t lastSlash = path.find_last_of("/\\");
		lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		size_t lastDot = path.rfind('.');
		size_t length = (lastDot == std::string::npos) ? path.size() - lastSlash : lastDot - lastSlash;
		m_name = path.substr(lastSlash, length);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) :
		m_name(name), m_rendererID(0)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(shaderSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_rendererID);
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

	std::string OpenGLShader::ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);

		BGE_ASSERT(file, "Could not open file: " << path);

		std::string result;

		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(result.data(), result.size());
		file.close();

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenSize = strlen(typeToken);
		size_t pos = source.find(typeToken);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			BGE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenSize + 1;
			std::string type = source.substr(begin, eol - begin);
			BGE_ASSERT(ShaderFromTypeString(type), "Invalid shader type");

			size_t	nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderFromTypeString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		BGE_ASSERT(shaderSources.size() <= 4, "You can use 2 shaders at most");

		std::array<GLuint, 2> glShaderIDs;

		// Compile given shaders
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

				BGE_PRINT("Failed to compile " << type << " shader!");
				BGE_ASSERT(false, infoLog.data());
				
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		// Link program
		glLinkProgram(program);

		// Check if linked correctly
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

			BGE_PRINT("Failed to link shaders!");
			BGE_ASSERT(false, infoLog.data());
			
			return;
		}
		
		// Detach all created shaders
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