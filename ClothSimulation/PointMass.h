#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

// Point mass struct
struct PointMass {
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 force;
	GLfloat mass;
};
