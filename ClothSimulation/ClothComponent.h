#pragma once

#include "PointMass.h"
#include "SpringConstraint.h"

#include <glad\glad.h>

#include <vector>

class Entity;
class Scene;

struct ClothComponent {
	std::vector<PointMass> pointMasses;

	// Spring constraints, bucketed by cloth patches top left pointmass index
	// Index -> o---o
	//          |\ /|
	//          | \ |   <-- Patch
	//          |/ \|
	//          o---o
	std::vector<std::vector<SpringConstraint>> springConstraints;
	
	GLuint numPointMassesX;
	GLuint numPointMassesY;

	PointMass& getPointMass(GLuint row, GLuint col);

	// Add sping contraint between two points on the cloth mesh
	// p1Row and p1Col can be used later to quickly lookup the constraints
	void addSpringConstraint(GLuint p1Row, GLuint p1Col, GLuint p2Row, GLuint p2Col, GLfloat stiffness, GLfloat breakDistance);

	// Check if there is a constraint between the point masses at the given indices.
	// idx1 is considered to be the index of the top left point mass in a cloth patch.
	// The point mass at idx2 must then be either to the right or down from the point mass at idx1.
	// Otherwise no constraint will be found, even if one exists.
	bool hasConstraintBetween(GLuint idx1, GLuint idx2) const;

	static Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea);	
};

namespace Prefabs {
	Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight);
}