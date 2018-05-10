#include "ScreenManager.h"

#include "Screen.h"

std::unique_ptr<Screen> g_currentScreen;

void ScreenManager::update()
{
	g_currentScreen->update();
}

void ScreenManager::switchScreen(std::unique_ptr<Screen> screen)
{
	g_currentScreen = std::move(screen);
}
