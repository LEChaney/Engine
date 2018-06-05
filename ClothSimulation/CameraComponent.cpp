#include "CameraComponent.h"

#include "Game.h"

#include <glm\gtc\matrix_transform.hpp>
#include <GLFW\glfw3.h>

CameraComponent::CameraComponent()
{
	int width, height;
	GLFWwindow* glContext = Game::getWindowContext();
	glfwGetFramebufferSize(glContext, &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	m_projectionMat = glm::perspective(glm::radians(60.0f), aspectRatio, 0.5f, 10000.0f);
}

void CameraComponent::setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	m_viewMat = glm::lookAt(pos, center, up);
	m_position = pos;
}

void CameraComponent::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	m_projectionMat = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	m_nearClipDistance = nearPlane;
	m_farClipDistance = farPlane;
}

glm::vec3 CameraComponent::getForward() const
{
	return glm::vec4{ 0, 0, -1, 0 } *m_viewMat;
}

glm::vec3 CameraComponent::getRight() const
{
	return glm::vec4{ 1, 0, 0, 0 } *m_viewMat;
}

glm::vec3 CameraComponent::getUp() const
{
	return glm::vec4{ 0, 1, 0, 0 } *m_viewMat;
}

GLfloat CameraComponent::getNearClipDistance() const
{
	return m_nearClipDistance;
}

GLfloat CameraComponent::getFarClipDistance() const
{
	return m_farClipDistance;
}

const glm::mat4& CameraComponent::getView() const
{
	return m_viewMat;
}

const glm::mat4& CameraComponent::getProjection() const
{
	return m_projectionMat;
}

const glm::vec3& CameraComponent::getPosition() const
{
	return m_position;
}