#include "ClothSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"

ClothSystem::ClothSystem(Scene& scene)
	: System(scene)
	, m_kNumConstraintSolverIterations{ 10 }
	, m_kDamping{ 0.01f }
	, m_kTimeStep{ 1.0f / 60.0f }
	, m_kTimeStepSq{ m_kTimeStep * m_kTimeStep }
{
}


ClothSystem::~ClothSystem()
{
}

Entity& ClothSystem::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea)
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
		if (i < 4)
			p.isFixed = true;

		cloth.pointMasses.push_back(p);
	}

	// Connect point masses with spring constraints
	for (GLuint r = 0; r < numPointsY - 1; ++r) {
		for (GLuint c = 0; c < numPointsX - 1; ++c) {
			// Structural Constraints
			cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r, c + 1), 1);
			cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r + 1, c), 1);

			// Shear Constraints
			cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r + 1, c + 1), 0.5);
			cloth.springConstraints.emplace_back(cloth.getPointMass(r, c + 1), cloth.getPointMass(r + 1, c), 0.5);

			// Bending Constraints
			if (c < numPointsX - 2)
				cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r, c + 2), 0.1);
			if (r < numPointsY - 2)
				cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r + 2, c), 0.1);
			if (r < numPointsY - 2 && c < numPointsX - 2) {
				cloth.springConstraints.emplace_back(cloth.getPointMass(r, c), cloth.getPointMass(r + 2, c + 2), 0.01);
				cloth.springConstraints.emplace_back(cloth.getPointMass(r, c + 2), cloth.getPointMass(r + 2, c), 0.01);
			}
		}
	}

	return clothEntity;
}

void ClothSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		
		// TODO: Add code for fixed time step

		if (entity.hasComponents(COMPONENT_CLOTH)) {
			ClothComponent& cloth = entity.cloth;

			// Solve constraints
			for (GLuint i = 0; i < m_kNumConstraintSolverIterations; ++i) {
				for (SpringConstraint& constraint : cloth.springConstraints) {
					constraint.solveConstraint();
				}
			}

			for (PointMass& pointMass : cloth.pointMasses) {
				pointMass.force += pointMass.mass * glm::vec3{ 0, -9.81f, 0 };

				// Integrate pointmasses
				glm::vec3 tmp = pointMass.getPosition();
				glm::vec3 acceleration = pointMass.force / pointMass.mass;
				pointMass.addOffset((pointMass.getPosition() - pointMass.prevPosition) * (1.0f - m_kDamping) + acceleration * m_kTimeStepSq);
				pointMass.prevPosition = tmp;
				pointMass.force = { 0, 0, 0 };
			}

			// Update vertices on GPU
			glBindBuffer(GL_ARRAY_BUFFER, entity.model.meshes.at(0).VBO);
			auto vertices = static_cast<VertexFormat*>(glMapBufferRange(
				GL_ARRAY_BUFFER,
				0, 
				sizeof(VertexFormat) * cloth.pointMasses.size(),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
			);
			for (GLuint i = 0; i < cloth.pointMasses.size(); ++i) {
				vertices[i].position = cloth.pointMasses[i].getPosition();
				vertices[i].normal = { 0, 1, 0 };
				vertices[i].texCoord = { 
					(i % cloth.numPointMassesX) / static_cast<float>(cloth.numPointMassesX), 
					(i / cloth.numPointMassesX) / static_cast<float>(cloth.numPointMassesY) 
				};
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
	}
}

void ClothSystem::beginFrame()
{
}

void ClothSystem::endFrame()
{
}


Entity& Prefabs::createCloth(Scene& scene, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat totalWeight)
{
	return ClothSystem::createCloth(scene, numPointsX, numPointsY, width, height, totalWeight);
}