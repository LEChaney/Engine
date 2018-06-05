#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "TransformComponent.h"
struct PyramidCollisionComponent {
	std::vector<glm::vec3>& GetPoints(TransformComponent& pyramidTransform);
};