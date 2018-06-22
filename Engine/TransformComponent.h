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

struct TransformComponent {
	TransformComponent();

	glm::vec3 position;
	glm::vec3 eulerAngles;
	glm::vec3 scale;
};