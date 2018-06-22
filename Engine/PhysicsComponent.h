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

struct PhysicsComponent {
	PhysicsComponent();

	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool gravityEnabled;
};