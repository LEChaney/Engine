#pragma once

#include "PointMass.h"
#include "SpringConstraint.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <array>

class Entity;
class Scene;

enum LinkDirection : size_t {
	LINK_NORTH,
	LINK_NORTH_EAST,
	LINK_EAST,
	LINK_SOUTH_EAST,
	LINK_SOUTH,
	LINK_SOUTH_WEST,
	LINK_WEST,
	LINK_NORTH_WEST
};

struct ClothLink {
	ClothLink(GLuint node1Id, GLuint node2Id, GLuint springConstraintId);

	GLuint node1Id;
	GLuint node2Id;
	GLuint springConstraintId;
};

struct ClothNode {
	ClothNode(glm::vec3 position, GLfloat mass, GLboolean isFixed);

	PointMass pointMass;
	// Cloth links bucketed by cardinal direction (north, north east, ... , north west)
	std::array<std::vector<ClothLink>, 8> linkDirections;
};

class ClothComponent {
public:

	// Add sping contraint between two points on the cloth mesh.
	void addClothLink(GLuint pointMass1Id, GLuint pointMass2Id, GLfloat stiffness, GLfloat breakDistance);

	void breakClothLink(const ClothLink& clothLink);
	void breakAllLinksInDirection(GLuint nodeId, LinkDirection direction);

	// Check if there is a constraint between the point masses at the given indices.
	// idx1 is considered to be the index of the top left point mass in a cloth patch.
	// The point mass at idx2 must then be either to the right or down from the point mass at idx1.
	// Otherwise no constraint will be found, even if one exists.
	bool hasConstraintBetween(GLuint idx1, GLuint idx2) const;

	static Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea);

private:
	std::vector<SpringConstraint> m_springConstraints;
	std::vector<ClothNode> m_clothNodes;
	std::vector<GLuint> m_triIndices;

	GLuint m_numPointMassesX;
	GLuint m_numPointMassesY;
};

namespace Prefabs {
	Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight);
}