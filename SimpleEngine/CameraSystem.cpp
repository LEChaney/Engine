#include "CameraSystem.h"

#include "Entity.h"

#include <glm\glm.hpp>

using glm::vec3;

CameraSystem::CameraSystem(Scene& scene, std::vector<Entity*>& playerList)
	: System(scene)
	, m_playerList{ playerList }
{
}


CameraSystem::~CameraSystem()
{
}

void CameraSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_CAMERA))
		return;

	if (m_playerList.size() > 0) {
		float minX = m_playerList[0]->transform.position.x;
		float maxX = minX;
		float minZ = m_playerList[0]->transform.position.y;
		float maxZ = minZ;
		for (auto player : m_playerList) {
			const vec3& position = player->transform.position;
			minX = glm::min(minX, position.x);
			maxX = glm::max(maxX, position.x);
			minZ = glm::min(minZ, position.z);
			maxZ = glm::max(maxZ, position.z);
		}
		vec3 center = { (minX + maxX) / 2, 0, (minZ + maxZ) / 2 };

		entity.camera.setLookAt(center + vec3{ 0, 100, 10 }, center, vec3{ 0, 1, 0 });
	}
}

void CameraSystem::beginFrame()
{
}

void CameraSystem::endFrame()
{
}
