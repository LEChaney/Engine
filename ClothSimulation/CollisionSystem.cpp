#include "CollisionSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"
#include "GLPrimitives.h"

CollisionSystem::CollisionSystem(Scene& scene)
	: System(scene)
{
}


CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& clothEntity = m_scene.getEntity(i);
		if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
			for (size_t j = 0; j < m_scene.getEntityCount(); ++j) {
				Entity& shapeEntity = m_scene.getEntity(j);
				if (shapeEntity.hasComponents(COMPONENT_SPHERE_COLLISION)) {
					SphereCollision(clothEntity, shapeEntity);
				}
				else if (shapeEntity.hasComponents(COMPONENT_PYRAMID_COLLISION)) {
					PyramidCollision(clothEntity, shapeEntity);
				}
			}
		}
	}
}

void CollisionSystem::beginFrame()
{
}

void CollisionSystem::endFrame()
{
}

void CollisionSystem::SphereCollision(Entity& clothEntity, Entity& sphereEntity)
{
	for (auto& clothNode : clothEntity.cloth.clothNodes) {
		glm::vec3 displacement = clothNode.pointMass.getPosition() - sphereEntity.transform.position;
		if (glm::length(displacement) < sphereEntity.sphereCollision.radius + 0.04f) {
			glm::vec3 moveDirection = glm::normalize(displacement);
			clothNode.pointMass.setPosition(sphereEntity.transform.position + (moveDirection * (sphereEntity.sphereCollision.radius + 0.04f)));
		}
	}
}

void CollisionSystem::PyramidCollision(Entity& clothEntity, Entity& pyramidEntity)
{
	auto pyramidPoints = pyramidEntity.pyramidCollision.GetPoints(pyramidEntity.transform);
	auto& pyramidIndicies = GLPrimitives::getPyramidIndices();
	auto& clothNodes = clothEntity.cloth.clothNodes;
	glm::vec3 pyramidScale = pyramidEntity.transform.scale;
	float maxCollisionDistance = glm::max(glm::max(pyramidScale.x, pyramidScale.y), pyramidScale.z);

	for (size_t i = 0; i < clothNodes.size()- clothEntity.cloth.getNumPointMassesX(); ++i) {			// Don't need the last row
		PointMass& pointMass = clothEntity.cloth.clothNodes.at(i).pointMass;
		glm::vec3 displacement = pointMass.getPosition() - pyramidEntity.transform.position;
		if (glm::length(displacement) < maxCollisionDistance && (i + 1) % clothEntity.cloth.getNumPointMassesX() != 0) {	// Ignore final pointmass in row
			for (size_t j = 3; j < pyramidPoints.size(); j += 3){	//TODO: Get ask lance about the indicies he added
				// Triangle 1
				glm::vec3 clothPoint1 = clothNodes.at(i).pointMass.getPosition();
				glm::vec3 clothPoint2 = clothNodes.at(i + 1).pointMass.getPosition();
				glm::vec3 clothPoint3 = clothNodes.at(i + clothEntity.cloth.getNumPointMassesX()).pointMass.getPosition();
				// Triangle 2
				glm::vec3 clothPoint4 = clothNodes.at(i + 1).pointMass.getPosition();
				glm::vec3 clothPoint5 = clothNodes.at(i + clothEntity.cloth.getNumPointMassesX()).pointMass.getPosition();
				glm::vec3 clothPoint6 = clothNodes.at(i + clothEntity.cloth.getNumPointMassesX() + 1).pointMass.getPosition();
				// Triangle 3
				glm::vec3 pyramidPoint1 = pyramidPoints.at(j);
				glm::vec3 pyramidPoint2 = pyramidPoints.at(j - 1);
				glm::vec3 pyramidPoint3 = pyramidPoints.at(j - 2);
			}
		}
	}
}
