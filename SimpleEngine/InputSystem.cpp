//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles input and propogates
//                input to entities.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//


#include "InputSystem.h"

#include "KeyObserver.h"
#include "Scene.h"
#include "Utils.h"
#include "Game.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>

InputSystem::InputSystem(Scene& scene)
	: System{ scene }
{

}

void InputSystem::beginFrame()
{
	static glm::dvec2 lastMousePos;
	GLFWwindow* window = glfwGetCurrentContext();

	// Set previous mouse pos to current mouse pos on first run
	static bool firstRun = true;
	if (firstRun) {
		glfwGetCursorPos(window, &lastMousePos.x, &lastMousePos.y);
		firstRun = false;
	}

	// Update input from mouse
	glm::dvec2 mousePos;
	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

	// Update mouse delta
	m_mouseDelta = mousePos - lastMousePos;

	lastMousePos = mousePos;
}

void InputSystem::update(Entity& entity)
{
	GLFWwindow* window = Game::getWindowContext();

	// DEBUG!!!
	if (entity.hasComponents(COMPONENT_MODEL)) {
		if (glfwGetKey(window, GLFW_KEY_KP_MULTIPLY) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.metallicness = clamp(entity.model.materials.at(i).shaderParams.metallicness + 0.01f, 0.001f, 1.0f);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_KP_DIVIDE) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.metallicness = clamp(entity.model.materials.at(i).shaderParams.metallicness - 0.01f, 0.001f, 1.0f);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness + 0.01f, 0.0001f, 1.0f);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness - 0.01f, 0.0001f, 1.0f);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.specBias = clamp(entity.model.materials.at(i).shaderParams.specBias + 0.01f, 0.0f, 0.96f);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.specBias = clamp(entity.model.materials.at(i).shaderParams.specBias - 0.01f, 0.0f, 0.9599f);
			}
		}
	}

	// Filter input receivers
	const size_t kInputReceiverMask = COMPONENT_INPUT | COMPONENT_INPUT_MAP;
	if (!entity.hasComponents(kInputReceiverMask))
		return;

	InputComponent& input = entity.input;
	InputMapComponent& inputMap = entity.inputMap;
	int gamepadIdx = inputMap.gamepadIdx;

	// Update input from axes
	int count;
	const float* pAxes = glfwGetJoystickAxes(gamepadIdx, &count);
	if (count > 0) {
		std::vector<float> axes(pAxes, pAxes + count);
		input.turnAxis = axes[inputMap.turnAxisMap];
	}

	// Update input from buttons
	const unsigned char* pBtns = glfwGetJoystickButtons(gamepadIdx, &count);
	if (count > 0) {
		std::vector<unsigned char> btns(pBtns, pBtns + count);
		input.acceleratorDown = btns[inputMap.accelerationBtnMap];
		input.brakeDown = btns[inputMap.brakeBtnMap];
	}

	// Update input from mouse
	input.orientationDelta = {};
	if (inputMap.mouseInputEnabled)
		input.orientationDelta = glm::vec3{ m_mouseDelta, 0 };

	// Update input from buttons
	input.axis = {};
	if (inputMap.leftBtnMap && glfwGetKey(window, inputMap.leftBtnMap) == GLFW_PRESS)
		input.axis.x -= 1;
	if (inputMap.rightBtnMap && glfwGetKey(window, inputMap.rightBtnMap) == GLFW_PRESS)
		input.axis.x += 1;
	if (inputMap.forwardBtnMap && glfwGetKey(window, inputMap.forwardBtnMap) == GLFW_PRESS)
		input.axis.z += 1;
	if (inputMap.backwardBtnMap && glfwGetKey(window, inputMap.backwardBtnMap) == GLFW_PRESS)
		input.axis.z -= 1;
	if (inputMap.downBtnMap && glfwGetKey(window, inputMap.downBtnMap) == GLFW_PRESS)
		input.axis.y -= 1;
	if (inputMap.upBtnMap && glfwGetKey(window, inputMap.upBtnMap) == GLFW_PRESS)
		input.axis.y += 1;
	if (inputMap.azimuthPosBtnMap && glfwGetKey(window, inputMap.azimuthPosBtnMap) == GLFW_PRESS)
		input.orientationDelta.x += 1;
	if (inputMap.azimuthNegBtnMap && glfwGetKey(window, inputMap.azimuthNegBtnMap) == GLFW_PRESS)
		input.orientationDelta.x -= 1;
	if (inputMap.elevationPosBtnMap && glfwGetKey(window, inputMap.elevationPosBtnMap) == GLFW_PRESS)
		input.orientationDelta.y += 1;
	if (inputMap.elevationNegBtnMap && glfwGetKey(window, inputMap.elevationNegBtnMap) == GLFW_PRESS)
		input.orientationDelta.y -= 1;
	if (inputMap.rollBtnMap && glfwGetKey(window, inputMap.rollBtnMap) == GLFW_PRESS)
		input.orientationDelta.z += 1; 
}