#include "Screen.h"

#include "Game.h"

Screen::Screen()
	: m_uiScreen{ new nanogui::Screen() }
{
	GLFWwindow* window = Game::getWindowContext();
	m_uiScreen->initialize(window, true);
	glfwSetWindowUserPointer(window, &m_uiScreen);

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *, double x, double y) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->cursorPosCallbackEvent(x, y);
	});

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->mouseButtonCallbackEvent(button, action, modifiers);
	});

	glfwSetKeyCallback(window,
		[](GLFWwindow *, int key, int scancode, int action, int mods) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->keyCallbackEvent(key, scancode, action, mods);
	});

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->charCallbackEvent(codepoint);
	});

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->dropCallbackEvent(count, filenames);
	});

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->scrollCallbackEvent(x, y);
	});

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		auto uiScreen = *static_cast<nanogui::ref<nanogui::Screen>*>(glfwGetWindowUserPointer(Game::getWindowContext()));
		uiScreen->resizeCallbackEvent(width, height);
	});
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
