#pragma once

#include <glm\glm.hpp>

struct TransformComponent {
	TransformComponent();

	glm::vec3 position;
	glm::vec3 eulerAngles;
	glm::vec3 scale;
};