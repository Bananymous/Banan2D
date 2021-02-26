#pragma once

#include "Camera.h"

#include "Banan/Core/Timestep.h"

#include "Banan/Event/WindowEvent.h"
#include "Banan/Event/MouseEvent.h"

namespace Banan
{

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera()				{ return m_camera; }
		const OrthographicCamera& GetCamera() const	{ return m_camera; }

	private:
		bool OnMouseScroll(MouseScrollEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool m_rotation;

		float m_aspectRatio;
		float m_zoomLevel			= 1.0f;

		float m_cameraRotation		= 0.0f;
		glm::vec3 m_cameraPosition	= { 0.0f, 0.0f, 0.0f };
		float m_cameraRotationSpeed	= 4.0f;
		float m_cameraMoveSpeed		= 2.0f;

		OrthographicCamera m_camera;

	};

}