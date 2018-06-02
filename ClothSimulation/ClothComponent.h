#pragma once

#include "PointMass.h"
#include "SpringConstraint.h"

#include <glad\glad.h>

#include <vector>

struct ClothComponent {
	std::vector<PointMass> pointMasses;
	std::vector<SpringConstraint> springConstraints;
	GLuint numPointMassesX;
	GLuint numPointMassesY;

	PointMass& getPointMass(GLuint row, GLuint col);
};