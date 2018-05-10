#include "Screen.h"

void Screen::update()
{
	for (auto& system : m_activeSystems) {
		system->beginFrame();
	}

	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		for (auto& system : m_activeSystems) {
			system->update(m_scene.getEntity(i));
		}
	}

	for (auto& system : m_activeSystems) {
		system->endFrame();
	}
}
