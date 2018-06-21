#pragma once

#include <glm\glm.hpp>

struct ParticleFormat {
	glm::vec4 positionAndRemainingLifetime;
	glm::vec4 velocityAndInitLifetime;
	glm::vec4 acceleration;
};