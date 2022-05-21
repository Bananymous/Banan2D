#include "bgepch.h"
#include "CameraController.h"

#include "../Core/Input.h"

namespace Banan
{

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) :
		m_camera(-aspectRatio * m_zoomLevel, aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel),
		m_rotation(rotation),
		m_aspectRatio(aspectRatio)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		float cam_sin = sin(m_cameraRotation)* m_cameraMoveSpeed* m_zoomLevel* ts;
		float cam_cos = cos(m_cameraRotation) * m_cameraMoveSpeed * m_zoomLevel * ts;

		if (Input::IsKeyPressed(Key::W))
		{
			m_cameraPosition.x -= cam_sin;
			m_cameraPosition.y += cam_cos;
		}
		if (Input::IsKeyPressed(Key::A))
		{
			m_cameraPosition.x -= cam_cos;
			m_cameraPosition.y -= cam_sin;
		}
		if (Input::IsKeyPressed(Key::S))
		{
			m_cameraPosition.x += cam_sin;
			m_cameraPosition.y -= cam_cos;
		}
		if (Input::IsKeyPressed(Key::D))
		{
			m_cameraPosition.x += cam_cos;
			m_cameraPosition.y += cam_sin;
		}
		m_camera.SetPosition(m_cameraPosition);

		if (m_rotation)
		{
			if (Input::IsKeyPressed(Key::Q))
				m_cameraRotation -= m_cameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::E))
				m_cameraRotation += m_cameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::Z))
				m_cameraRotation = 0.0f;
			m_camera.SetRotation(m_cameraRotation);
		}
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>	(BANAN_BIND_EVENT_FN(OrthographicCameraController::OnMouseScroll));
		dispatcher.Dispatch<WindowResizeEvent>	(BANAN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
	}

	bool OrthographicCameraController::OnMouseScroll(MouseScrollEvent& e)
	{
		m_zoomLevel *= (1.0f - e.GetOffset() * 0.25f);
		m_zoomLevel = std::clamp(m_zoomLevel, 0.25f, 20.0f);
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

}