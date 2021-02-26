#pragma once

#include <BananGameEngine.h>

class SandboxLayer : public Banan::Layer
{
public:
	SandboxLayer();
	~SandboxLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Banan::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Banan::Event& e) override;

private:
	Banan::OrthographicCameraController m_cameraController;

	Banan::QuadProperties m_quad;

	glm::vec4 m_color	= { 1.0f, 1.0f, 0.0f, 1.0f };
	uint8_t m_index		= 0;
	int8_t m_dir		= -1;

	float m_rotation	= 0.0f;

	bool m_FPSbool		= false;
	uint32_t m_FPS		= 0U;
	uint32_t m_frames	= 0U;
	float m_time		= 0.0f;
};

