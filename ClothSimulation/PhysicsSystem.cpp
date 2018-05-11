#define _USE_MATH_DEFINES

#include "PhysicsSystem.h"

#include "Clock.h"

PhysicsSystem::PhysicsSystem(Scene& scene)
	: System{ scene }
{
}

void PhysicsSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_PHYSICS, COMPONENT_TRANSFORM))
		return;

	float defaultDrag = 0.1f;
	entity.physics.velocity += (entity.physics.acceleration - entity.physics.velocity * defaultDrag) * Clock::getDeltaTime();
	entity.transform.position += entity.physics.velocity * Clock::getDeltaTime();

	entity.physics.acceleration = { 0, 0, 0 };

	//RenderSystem::drawDebugArrow(entity.lookAt[3], entity.physics.velocity, glm::length(entity.physics.velocity), { 0, 1, 0 });
	//RenderSystem::drawDebugArrow(glm::vec3(entity.lookAt[3]) + entity.physics.velocity, entity.physics.acceleration, glm::length(entity.physics.acceleration));
}
