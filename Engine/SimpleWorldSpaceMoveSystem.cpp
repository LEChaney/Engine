//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "SimpleWorldSpaceMoveSystem.h"

#include "Entity.h"
#include "Clock.h"

#include "GLMUtils.h"

#include <glm\glm.hpp>

SimpleWorldSpaceMoveSystem::SimpleWorldSpaceMoveSystem(Scene& scene)
	: System(scene)
{
}

void SimpleWorldSpaceMoveSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);

		if (!entity.hasComponents(COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT, COMPONENT_INPUT))
			continue;

		glm::vec3 axis = GLMUtils::limitVec(entity.input.axis, 1);
		float moveSpeed = entity.simpleWorldSpaceMovement.moveSpeed;

		entity.transform.position.x += axis.x * moveSpeed * Clock::getDeltaTime();
		entity.transform.position.z -= axis.z * moveSpeed * Clock::getDeltaTime();
	}
}

void SimpleWorldSpaceMoveSystem::beginFrame()
{
}

void SimpleWorldSpaceMoveSystem::endFrame()
{
}
