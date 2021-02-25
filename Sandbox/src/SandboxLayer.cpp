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
	m_quad.position.z = 1.0f;
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



	Banan::Renderer2D::ResetStats();

	Banan::Renderer2D::BeginScene(m_cameraController.GetCamera());

	float tileSize = 0.1f;
	for (float y = -5.0f; y < 5.0f; y += tileSize)
		for (float x = -5.0f; x < 5.0f; x += tileSize)
			Banan::Renderer2D::DrawQuad({ x, y, 0.0f }, glm::vec2(tileSize), { (x + 5.0f) / 10.f, (y + 5.0f) / 10.f, 0.3f, 1.0f });

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
	auto stats = Banan::Renderer2D::GetStats();

	ImGui::Begin("Statistics:");

	ImGui::Text("Draw Calls %d", stats.drawCalls);
	ImGui::Text("Quads %d", stats.quads);
	ImGui::Text("Vertices %d", stats.GetVertices());
	ImGui::Text("Indices %d", stats.GetIndices());

	ImGui::End();
}

void SandboxLayer::OnEvent(Banan::Event& e)
{
	m_cameraController.OnEvent(e);
}
