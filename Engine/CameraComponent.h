#pragma once

#include <glm\glm.hpp>

#include <glad\glad.h>

struct CameraComponent {
public:
	CameraComponent();

	void setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up);
	void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	const glm::mat4& getView() const;
	const glm::mat4& getProjection() const;
	const glm::vec3& getPosition() const;
	glm::vec3 getForward() const;
	glm::vec3 getRight() const;
	glm::vec3 getUp() const;
	GLfloat getNearClipDistance() const;
	GLfloat getFarClipDistance() const;

private:
	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;
	glm::vec3 m_position;
	GLfloat m_nearClipDistance;
	GLfloat m_farClipDistance;
};