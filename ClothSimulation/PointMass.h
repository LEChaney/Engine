#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

// Point mass struct
struct PointMass {
public:
	PointMass(glm::vec3 position, GLfloat mass, GLboolean isFixed);

	const glm::vec3& getPosition();
	void addOffset(glm::vec3 offset);

	glm::vec3 prevPosition;
	glm::vec3 force;
	GLfloat mass;
	GLboolean isFixed;

private:
	glm::vec3 m_position;
};
