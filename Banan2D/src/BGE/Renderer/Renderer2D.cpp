#include "bgepch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Banan
{

	struct Renderer2DStorage
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> shader;
		Ref<Texture2D> whiteTexture;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::Init()
	{
		RenderCommand::Init();

		s_data = new Renderer2DStorage();

		s_data->quadVertexArray = VertexArray::Create();

		float quadVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		auto quadVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "u_Position" },
			{ ShaderDataType::Float2, "u_TexCoord" }
		});
		s_data->quadVertexArray->AddVertexBuffer(quadVertexBuffer);

		uint32_t quadIndices[] = { 0, 1, 2, 2, 0, 3 };
		auto quadIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
		s_data->quadVertexArray->SetIndexBuffer(quadIndexBuffer);

		s_data->whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_data->shader = Shader::Create("assets/shaders/Shader2D.glsl");
		s_data->shader->Bind();
		s_data->shader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_data;
	}

	void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_data->shader->Bind();
		s_data->shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}
	 
	void Renderer2D::EndScene()
	{

	}



	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.color = color;
		DrawQuad(props);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.texture = texture;
		DrawQuad(props);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.rotation = rotation;
		props.color = color;
		DrawRotatedQuad(props);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
	{
		QuadPropreties props;
		props.position = position;
		props.size = size;
		props.rotation = rotation;
		props.texture = texture;
		DrawRotatedQuad(props);
	}

	void Renderer2D::DrawQuad(const QuadPropreties& props)
	{
		props.texture ? props.texture->Bind() : s_data->whiteTexture->Bind();

		s_data->shader->SetFloat4("u_Color", props.color);

		s_data->shader->SetMat4("u_Transform", glm::scale(glm::translate(glm::mat4(1.0f), props.position), glm::vec3(props.size, 1.0f)));

		s_data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->quadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const QuadPropreties& props)
	{
		props.texture ? props.texture->Bind() : s_data->whiteTexture->Bind();

		s_data->shader->SetFloat4("u_Color", props.color);

		s_data->shader->SetMat4("u_Transform", glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), props.position), props.rotation, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(props.size, 1.0f)));

		s_data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->quadVertexArray);
	}


}