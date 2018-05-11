#include "SimpleWorldSpaceMoveSystem.h"

#include "Entity.h"
#include "Clock.h"

#include "GLMUtils.h"

#include <glm\glm.hpp>

SimpleWorldSpaceMoveSystem::SimpleWorldSpaceMoveSystem(Scene& scene)
	: System(scene)
{
}

void SimpleWorldSpaceMoveSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT, COMPONENT_INPUT))
		return;

	glm::vec3 axis = GLMUtils::limitVec(entity.input.axis, 1);
	float moveSpeed = entity.simpleWorldSpaceMovement.moveSpeed;

	entity.transform.position.x += axis.x * moveSpeed * Clock::getDeltaTime();
	entity.transform.position.z -= axis.z * moveSpeed * Clock::getDeltaTime();
}

void SimpleWorldSpaceMoveSystem::beginFrame()
{
}

void SimpleWorldSpaceMoveSystem::endFrame()
{
}
