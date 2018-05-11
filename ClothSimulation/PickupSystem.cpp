#define _USE_MATH_DEFINES

#include "PickupSystem.h"

#include "Entity.h"
#include "Clock.h"
#include "PrimitivePrefabs.h"

#include <iostream>

PickupSystem::PickupSystem(Scene& scene, std::vector<Entity*>& playerList)
	: System{ scene }
	, m_playerList{playerList}
{
}

void PickupSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_PICKUP, COMPONENT_TRANSFORM))
		return;
	// The pick up can be picked up
	if (entity.pickup.isActive)
	{
		entity.transform.eulerAngles.y += 2 * Clock::getDeltaTime();

		// Checks if a player and the pickup collide
		for (int i = 0; i < m_playerList.size(); ++i)
		{

			if (m_playerList[i]->transform.position.x >= entity.transform.position.x - 2 && m_playerList[i]->transform.position.x <= entity.transform.position.x + 2)
			{
				if (m_playerList[i]->transform.position.z >= entity.transform.position.z - 2 && m_playerList[i]->transform.position.z <= entity.transform.position.z + 2)
				{
					m_playerList[i]->playerStats.numOfTails += 1;
					entity.pickup.isActive = false;
					entity.pickup.respawnTimeStamp = glfwGetTime();

					// Spawn a tail for the player
					TransformComponent snakeTailTransform{};
					Entity& snakeTail = Prefabs::createCube(m_scene, snakeTailTransform);
					snakeTail.transform.position = m_playerList[i]->transform.position;
					snakeTail.addComponents(COMPONENT_SNAKETAIL, COMPONENT_PHYSICS);
					snakeTail.snakeTail.entityToFollow = m_playerList[i];
					if(snakeTail.snakeTail.entityToFollow != NULL)
						std::cout << snakeTail.snakeTail.entityToFollow;
				}
			}
		}
	}
	// The pick up is respawning
	else if (glfwGetTime() >=  entity.pickup.respawnTimeStamp + entity.pickup.respawnTimeOffset)
	{
		entity.pickup.isActive = true;
	}
}
