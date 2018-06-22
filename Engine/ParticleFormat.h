//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glm\glm.hpp>

struct ParticleFormat {
	glm::vec4 positionAndRemainingLifetime;
	glm::vec4 velocity;
	glm::vec4 initialPositionAndLifetime;
	glm::vec4 initialVelocity;
};