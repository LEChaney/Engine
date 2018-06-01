#include "Cloth.h"

#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"

Entity& Cloth::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight)
{
	Entity& clothEntity = scene.createEntity(COMPONENT_TRANSFORM, COMPONENT_CLOTH);

	GLuint numPoints = numPointsX * numPointsY;
	GLfloat massOfOnePt = totalWeight / numPoints;

	std::vector<VertexFormat> vertices;
	std::vector<GLuint> indices;
	GLUtils::createTessellatedQuadData(numPointsX, numPointsY, width, height, vertices, indices);

	for (auto& vert : vertices) {
		PointMass p;
		p.prevPosition = p.position = vert.position;
		p.force = { 0, 0, 0 };
		p.mass = massOfOnePt;
	}

	return clothEntity;
}
