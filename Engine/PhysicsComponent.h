#pragma once

#include <glm\glm.hpp>

struct PhysicsComponent {
	PhysicsComponent();

	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool gravityEnabled;
};