#pragma once

#include <glm\glm.hpp>

struct ParticleFormat {
	glm::vec4 positionAndRemainingLifetime;
	glm::vec4 velocity;
	glm::vec4 initialPositionAndLifetime;
	glm::vec4 initialVelocity;
};