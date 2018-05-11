//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The main entry point for the program.
//                Handles executing the main loop.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "Game.h"
#include "Log.h"

#include <GLFW\glfw3.h>

int main()
{
	g_log.setOutputFile("Log.txt");

	Game::init();
	GLFWwindow* window = Game::getWindowContext();

	while (!glfwWindowShouldClose(window)) {		
		Game::executeOneFrame();
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}