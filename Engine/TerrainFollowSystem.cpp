#include "TerrainFollowSystem.h"

#include "Terrain.h"
#include "Clock.h"

#include <glm\gtx\compatibility.hpp>

TerrainFollowSystem::TerrainFollowSystem(Scene& scene)
	: System(scene)
{
}

void TerrainFollowSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);

		if (!entity.hasComponents(COMPONENT_TERRAIN_FOLLOW, COMPONENT_TRANSFORM, COMPONENT_PHYSICS))
			continue;

		// Fall onto terrain
		float terrainHeight;
		bool success = TerrainUtils::castPosToTerrainHeight(*entity.terrainFollow.terrainToFollow, entity.transform.position, terrainHeight);
		float halfHeight = entity.terrainFollow.followerHalfHeight;
		float yPos = entity.transform.position.y;
		float targetYPos = terrainHeight + halfHeight;
		if (success && yPos - targetYPos < 0.1f) {
			entity.physics.gravityEnabled = false;
			entity.transform.position.y = glm::lerp(yPos, targetYPos, Clock::getDeltaTime() * 50.0f);
			entity.physics.velocity.y = glm::max(entity.physics.velocity.y, 0.0f);

			// Jump
			if (entity.terrainFollow.jumpEnabled && entity.hasComponents(COMPONENT_INPUT) && entity.input.axis.y > 0) {
				entity.physics.velocity.y += 5.0f;
			}
		}
		else {
			entity.physics.gravityEnabled = true;
		}
	}
}

void TerrainFollowSystem::beginFrame()
{
}

void TerrainFollowSystem::endFrame()
{
}
