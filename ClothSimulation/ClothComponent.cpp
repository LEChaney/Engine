#include "ClothComponent.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"

PointMass& ClothComponent::getPointMass(GLuint row, GLuint col)
{
	return pointMasses.at(row * numPointMassesX + col);
}

void ClothComponent::addSpringConstraint(GLuint p1Row, GLuint p1Col, GLuint p2Row, GLuint p2Col, GLfloat stiffness, GLfloat breakDistance)
{
	if (springConstraints.size() < numPointMassesX * numPointMassesY) {
		springConstraints.resize(numPointMassesX * numPointMassesY);
	}

	springConstraints.at(p1Row * numPointMassesX + p1Col).emplace_back(
		pointMasses.at(p1Row * numPointMassesX + p1Col),
		pointMasses.at(p2Row * numPointMassesX + p2Col),
		stiffness,
		breakDistance
	);
}

bool ClothComponent::hasConstraintBetween(GLuint idx1, GLuint idx2) const
{
	auto& constraintsToSearch = springConstraints.at(idx1);
	const PointMass& pm1 = pointMasses.at(idx1);
	const PointMass& pm2 = pointMasses.at(idx2);

	for (auto& constraint : constraintsToSearch) {
		if ((&constraint.getPointMass1() == &pm1 && &constraint.getPointMass2() == &pm2)
		 || (&constraint.getPointMass1() == &pm2 && &constraint.getPointMass2() == &pm1))
			return true;
	}

	return false;
}

Entity& ClothComponent::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea)
{
	Entity& clothEntity = scene.createEntity(COMPONENT_CLOTH, COMPONENT_MODEL);
	ClothComponent& cloth = clothEntity.cloth;
	cloth.numPointMassesX = numPointsX;
	cloth.numPointMassesY = numPointsY;

	// Calculate each point mass's individual mass
	GLuint numPoints = numPointsX * numPointsY;
	GLfloat separationX = width / (numPointsX - 1);
	GLfloat separationY = height / (numPointsY - 1);
	GLfloat patchMass = separationX * separationY * weightPerUnitArea; // Weight for a 2 x 2 patch of point masses
	GLfloat individualMass = patchMass / 4.0f; // Assuming square patches

											   // Create a tessellated quad to build the cloth out of
	std::vector<VertexFormat> vertices;
	std::vector<GLuint> indices;
	GLUtils::createTessellatedQuadData(numPointsX, numPointsY, width, height, vertices, indices);

	// Buffer triangle mesh to GPU for rendering
	clothEntity.model.rootNode.meshIDs.push_back(0);
	clothEntity.model.meshes.push_back(GLUtils::bufferMeshData(vertices, indices, GL_STREAM_DRAW));

	// Set Material to use for drawing
	Material material;
	material.willDrawWireframe = true;
	clothEntity.model.materials.push_back(material);

	// Create point masses from cloth vertices 
	cloth.pointMasses.reserve(vertices.size());
	for (GLuint i = 0; i < vertices.size(); ++i) {
		PointMass p(vertices[i].position, individualMass, false);

		// Fix first 4 verts
		if (i < numPointsX)
			p.isFixed = true;

		cloth.pointMasses.push_back(p);
	}

	// Connect point masses with spring constraints
	for (GLuint r = 0; r < numPointsY - 1; ++r) {
		for (GLuint c = 0; c < numPointsX - 1; ++c) {
			GLuint i = r * numPointsX + c;

			// Structural Constraints
			cloth.addSpringConstraint(r, c, r, c + 1, 1.0f, 0.25f);
			cloth.addSpringConstraint(r, c, r + 1, c, 1.0f, 0.25f);

			// Shear Constraints
			cloth.addSpringConstraint(r, c, r + 1, c + 1, 0.5f, 0.25f);
			cloth.addSpringConstraint(r, c + 1, r + 1, c, 0.5f, 0.25f);

			// Bending Constraints
			if (c < numPointsX - 2)
				cloth.addSpringConstraint(r, c, r, c + 2, 0.1f, 0.25f);
			if (r < numPointsY - 2)
				cloth.addSpringConstraint(r, c, r + 2, c, 0.1f, 0.25f);
			if (r < numPointsY - 2 && c < numPointsX - 2) {
				cloth.addSpringConstraint(r, c, r + 2, c + 2, 0.1f, 0.25f);
				cloth.addSpringConstraint(r, c + 2, r + 2, c, 0.1f, 0.25f);
			}
		}
	}

	return clothEntity;
}

Entity& Prefabs::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight)
{
	return ClothComponent::createCloth(scene, numPointsX, numPointsY, width, height, totalWeight);
}