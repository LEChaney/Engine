#include "Screen.h"

#include "Game.h"

Screen::Screen()
	: m_uiScreen{ new nanogui::Screen() }
{
	m_uiScreen->initialize(Game::getWindowContext(), true);
}

void Screen::update()
{
	for (auto& system : m_activeSystems) {
		system->beginFrame();
	}

	for (auto& system : m_activeSystems) {
		system->update();
	}

	if (m_uiScreen) {
		m_uiScreen->drawContents();
		m_uiScreen->drawWidgets();
	}

	for (auto& system : m_activeSystems) {
		system->endFrame();
	}
}
