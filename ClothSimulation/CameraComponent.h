#pragma once

#include <glm\glm.hpp>

struct CameraComponent {
public:
	void setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up);
	const glm::mat4& getView() const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getForward() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;

private:
	glm::mat4 m_viewMat;
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;
};