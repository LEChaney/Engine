#include "CollisionSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"

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

void CollisionSystem::SphereCollision(Entity & clothEntity, Entity & sphereEntity)
{
	for (PointMass& pointMass : clothEntity.cloth.pointMasses) {
		glm::vec3 displacement = pointMass.getPosition() - sphereEntity.transform.position;
		if (glm::length(displacement) < sphereEntity.sphereCollision.radius + 0.04f) {
			glm::vec3 moveDirection = glm::normalize(displacement);
			pointMass.setPosition(sphereEntity.transform.position + (moveDirection * (sphereEntity.sphereCollision.radius + 0.04f)));
		}
	}
}
