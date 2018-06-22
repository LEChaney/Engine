#include "AnimationSystem.h"

AnimationSystem::AnimationSystem(Scene& scene)
	: System(scene)
{
}

void AnimationSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);

		if (!entity.hasComponents(COMPONENT_ANIMATED_MODEL, COMPONENT_INPUT))
			continue;

		// Handle in air animations
		if (entity.hasComponents(COMPONENT_TERRAIN_FOLLOW) && !entity.terrainFollow.isOnGround) {
			if (entity.animatedModel.model->bFalling == false) {
				entity.animatedModel.model->bFalling = true;
				entity.animatedModel.model->bMoving = false;
				entity.animatedModel.model->setCurrentAnimation(71, 80); // Fall animation?
			}
		}
		// Handle ground animations
		else {
			// Stop Falling animations
			if (entity.animatedModel.model->bFalling == true) {
				entity.animatedModel.model->bFalling = false;
				entity.animatedModel.model->setCurrentAnimation(0, 30);
			}

			// Handle ground movement
			if (entity.input.axis.z != 0 || entity.input.axis.x != 0) {
				if (entity.animatedModel.model->bMoving == false) {
					entity.animatedModel.model->bMoving = true;
					entity.animatedModel.model->setCurrentAnimation(31, 50); // run animation
				}
			}
			// Handle ground idle
			else {
				if (entity.animatedModel.model->bMoving == true) {
					entity.animatedModel.model->bMoving = false;
					entity.animatedModel.model->setCurrentAnimation(0, 30); //idle animation
				}
			}
		}
	}
}

void AnimationSystem::beginFrame()
{
}

void AnimationSystem::endFrame()
{
}
