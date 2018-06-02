#include "Screen.h"

void Screen::update()
{
	for (auto& system : m_activeSystems) {
		system->beginFrame();
	}

	for (auto& system : m_activeSystems) {
		system->update();
	}

	for (auto& system : m_activeSystems) {
		system->endFrame();
	}
}
