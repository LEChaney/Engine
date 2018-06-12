#pragma once

#include "PointMass.h"
#include "SpringConstraint.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <array>
#include <list>

class Entity;
class Scene;

enum LinkDirection : GLint {
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
	ClothLink(GLuint node1Id, GLuint node2Id, GLint linkDirection, const SpringConstraint& springConstraint);
	
	GLuint node1Id;
	GLuint node2Id;
	GLint direction;
	SpringConstraint springConstraint;
};

using ClothLinkIterator = std::list<ClothLink>::iterator;

struct ClothNode {
	ClothNode(glm::vec3 position, GLfloat mass, GLboolean isFixed);

	PointMass pointMass;
	// Cloth links bucketed by cardinal direction (north, north east, ... , north west)
	std::array<std::vector<ClothLinkIterator>, 8> linkDirections;
};

class ClothComponent {
public:

	// Add sping contraint between two points on the cloth mesh.
	void addClothLink(GLuint node1Id, GLuint node2Id, GLfloat stiffness, GLfloat breakDistance);

	ClothNode& getNode(GLuint id);
	GLuint getNumClothNodes() const;
	GLuint getNumClothLinks() const;

	GLuint getNumPointMassesX() const;
	GLuint getNumPointMassesY() const;

	void breakStructualLink(ClothLinkIterator clothLink);
	void breakAllLinksFromNodeInDirection(GLuint nodeId, GLint direction);

	// Check if there is a constraint between the point masses at the given indices.
	// idx1 is considered to be the index of the top left point mass in a cloth patch.
	// The point mass at idx2 must then be either to the right or down from the point mass at idx1.
	// Otherwise no constraint will be found, even if one exists.
	bool hasConstraintBetween(GLuint idx1, GLuint idx2) const;

	static Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea, GLint linkDistance);

	std::vector<ClothNode> clothNodes;
	std::vector<GLuint> triIndices;
	std::list<ClothLink> clothLinks;
	GLint clothPoints;
	GLint maxClothPoints;

private:
	GLuint m_numPointMassesX;
	GLuint m_numPointMassesY;

	GLint wrapDirection(GLint direction);
	GLint getReverseDirection(GLint direction);
};

namespace Prefabs {
	Entity& createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight, GLint linkDistance);
}