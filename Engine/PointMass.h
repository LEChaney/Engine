#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

// Point mass struct
struct PointMass {
public:
	PointMass(glm::vec3 position, GLfloat mass, GLboolean isFixed);

	const glm::vec3& getPosition();
	void addOffset(const glm::vec3& offset);
	void setPosition(const glm::vec3& newPosition);

	glm::vec3 prevPosition;
	glm::vec3 force;
	GLfloat mass;
	GLboolean isFixed;

	// TODO: Move this to cloth node as it is cloth specific
	GLfloat burnedPercent;
	GLboolean isBurning;

private:
	glm::vec3 m_position;
};
