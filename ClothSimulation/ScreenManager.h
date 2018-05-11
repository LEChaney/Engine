#pragma once

#include <memory>

class Screen;

namespace ScreenManager {
	void update();
	void switchScreen(std::unique_ptr<Screen> screen);
}