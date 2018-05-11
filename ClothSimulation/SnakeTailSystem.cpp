#define _USE_MATH_DEFINES

#include "SnakeTailSystem.h"

#include "Entity.h"
#include "Clock.h"
#include "PrimitivePrefabs.h"
#include "Entity.h"
#include "AIUtils.h"

#include <iostream>

SnakeTailSystem::SnakeTailSystem(Scene& scene)
	: System{ scene }
{
}

void SnakeTailSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_SNAKETAIL))
		return;

	glm::vec3 acceleration = seekWithArrival(entity.snakeTail.entityToFollow->transform.position, entity.transform.position, entity.physics.velocity, entity.vehicleMovement.maxMoveSpeed);
	steer(entity, acceleration);
}
