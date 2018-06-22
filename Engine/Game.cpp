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

#include "Game.h"

#include "GLUtils.h"
#include "ScreenManager.h"
#include "Clock.h"
#include "GameplayScreen.h"

#include <GLFW\glfw3.h>

#include <nanogui/nanogui.h>

#include <memory>

GLFWwindow* g_window;

GLFWwindow* Game::getWindowContext()
{
	return g_window;
}

void Game::init()
{
	// Init combined Window and OpenGL context.
	g_window = GLUtils::initOpenGL();

	// Preload models and textures
	Game::preloadModelsAndTextures();

	ScreenManager::switchScreen(std::unique_ptr<Screen>(new GameplayScreen));
}

// Load game specific models and textures into GPU memory here
void Game::preloadModelsAndTextures()
{

}


void Game::executeOneFrame()
{
	Clock::update();
	ScreenManager::update();
}
