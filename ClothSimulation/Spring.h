#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

// Spring struct
struct Spring {
	glm::vec3& point1;
	glm::vec3& point2;
};