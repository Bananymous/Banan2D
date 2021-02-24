#include "bgepch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Banan
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) :
		m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
	{
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::CalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);
		transform = glm::rotate(transform, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_position = position; CalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(float rotation)
	{
		m_rotation = rotation; CalculateViewMatrix();
	}


}