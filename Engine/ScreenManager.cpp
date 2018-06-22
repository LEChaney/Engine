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
