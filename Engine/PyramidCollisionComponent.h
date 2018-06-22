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
#include <vector>
#include <glm\glm.hpp>
#include "TransformComponent.h"
struct PyramidCollisionComponent {
	std::vector<glm::vec3> GetPoints(TransformComponent& pyramidTransform);
};