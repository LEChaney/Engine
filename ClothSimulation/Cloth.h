#pragma once

#include "PointMass.h"
#include "SpringConstraint.h"

#include <glad\glad.h>

#include <vector>

class Entity;
class Scene;

struct ClothComponent {
	std::vector<PointMass> pointMasses;
	std::vector<SpringConstraint> springs;
};

namespace Cloth {
	Entity& createCloth(Scene&, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight);
}