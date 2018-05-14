#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

// Point mass struct (matches ComputeShader storage block)
struct PointMassFormat {
	glm::vec4 position;
	glm::vec4 velocity;
	glm::vec4 force;
	GLfloat mass;
	glm::vec3 _pad;
};
