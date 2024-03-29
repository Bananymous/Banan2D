#pragma once

#include <glm/glm.hpp>

namespace Banan
{

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position);
		void SetRotation(float rotation);

		const glm::vec3& GetPosition() const	{ return m_position; }
		float GetRotation() const				{ return m_rotation; }

		const glm::mat4& GetProjectionMatrix() const		{ return m_projectionMatrix; }
		const glm::mat4& GetViewMatrix() const				{ return m_viewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const	{ return m_viewProjectionMatrix; }

	private:
		void CalculateViewMatrix();

	private:
		glm::mat4 m_viewProjectionMatrix;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix = glm::mat4(1.0f);

		glm::vec3 m_position	= glm::vec3(0.0f);
		float m_rotation		= 0.0f;
	};

}