#include "SandboxLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

SandboxLayer::SandboxLayer() :
	Layer("Sandbox2D"), m_cameraController(16.0f / 9.0f, true)
{

}
 
void SandboxLayer::OnAttach()
{
	m_quad.size = glm::vec2(1.0f, 1.0f);
	m_quad.texture = Banan::Texture2D::Create("assets/textures/test.png");
	m_quad.rotation = 0.4f;
}

void SandboxLayer::OnDetach()
{
	
}

void SandboxLayer::OnUpdate(Banan::Timestep ts)
{
	m_cameraController.OnUpdate(ts);

	m_quad.tint = m_color;
	m_quad.rotation = m_rotation;

	Banan::RenderCommand::SetClearColor({ 0.0f, 1.0f, 1.0f, 1.0f });
	Banan::RenderCommand::Clear();

	Banan::Renderer2D::BeginScene(m_cameraController.GetCamera());

	for(int i = 0; i < 10; i++)
		Banan::Renderer2D::DrawQuad(Banan::QuadPropreties(glm::vec3(0.0f, (float)i * 0.11f, 0.0f), glm::vec2(0.1f, 0.1f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));

	Banan::Renderer2D::DrawRotatedQuad(m_quad);

	Banan::Renderer2D::EndScene();

	m_rotation += 2.0f * ts;

	// Hue shift
	float speed = 3.0f * ts;
	float& color = m_color[m_index];
	color += speed * m_dir;
	if (color >= 1.0f || color <= 0.0f)
	{
		color = std::clamp(color, 0.0f, 1.0f);
		m_dir *= -1;
		if (++m_index >= 3)
			m_index = 0;
	}

	// Calculate FPS
	m_time += ts;
	m_frames++;
	if (m_time >= 1.0f)
	{
		std::wstring new_title = L"Sandbox (FPS: " + std::to_wstring(m_frames) + L')';
		Banan::Application::Get().GetWindow().SetTitle(new_title);
		m_frames = 0U;
		m_time -= 1.0f;
	}
}

void SandboxLayer::OnImGuiRender()
{
	ImGui::Begin("System:");
	ImGui::Text("MOIIII <33");
	ImGui::ColorEdit4("", glm::value_ptr(m_color));
	ImGui::End();
}

void SandboxLayer::OnEvent(Banan::Event& e)
{
	m_cameraController.OnEvent(e);
}
