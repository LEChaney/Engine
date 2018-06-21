#define _USE_MATH_DEFINES

#include "PhysicsSystem.h"

#include "Clock.h"

PhysicsSystem::PhysicsSystem(Scene& scene)
	: System{ scene }
{
}

void PhysicsSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		if (!entity.hasComponents(COMPONENT_PHYSICS, COMPONENT_TRANSFORM))
			continue;

		float defaultDrag = 0.1f;
		entity.physics.velocity += (entity.physics.acceleration - entity.physics.velocity * defaultDrag) * Clock::getDeltaTime();
		entity.transform.position += entity.physics.velocity * Clock::getDeltaTime();

		if (entity.physics.gravityEnabled)
			entity.physics.acceleration = { 0, -9.81, 0 };
		else
			entity.physics.acceleration = { 0, 0, 0 };

		//RenderSystem::drawDebugArrow(entity.lookAt[3], entity.physics.velocity, glm::length(entity.physics.velocity), { 0, 1, 0 });
		//RenderSystem::drawDebugArrow(glm::vec3(entity.lookAt[3]) + entity.physics.velocity, entity.physics.acceleration, glm::length(entity.physics.acceleration));
	}
}
