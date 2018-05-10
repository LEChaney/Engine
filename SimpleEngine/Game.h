#pragma once

struct GLFWwindow;

namespace Game {
	void init();
	GLFWwindow* getWindowContext();
	void preloadModelsAndTextures();
	void executeOneFrame();
}
