#include "CameraComponent.h"

#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

void CameraComponent::setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	m_viewMat = lookAt(pos, center, up);
	m_position = pos;
	m_forward = normalize(center - pos);
	m_up = normalize(up);
	m_right = normalize(cross(m_forward, m_up));
}

const glm::mat4& CameraComponent::getView() const
{
	return m_viewMat;
}

const glm::vec3& CameraComponent::getPosition() const
{
	return m_position;
}

const glm::vec3& CameraComponent::getForward() const
{
	return m_forward;
}

const glm::vec3& CameraComponent::getRight() const
{
	return m_right;
}

const glm::vec3& CameraComponent::getUp() const
{
	return m_up;
}
