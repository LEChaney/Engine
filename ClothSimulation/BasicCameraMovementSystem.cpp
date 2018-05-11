//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement based on an
//                entities input state.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "BasicCameraMovementSystem.h"

#include "BasicCameraMovementComponent.h"
#include "GLUtils.h"
#include "GLMUtils.h"
#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

BasicCameraMovementSystem::BasicCameraMovementSystem(Scene& scene)
	: System(scene)
{
}

BasicCameraMovementSystem::~BasicCameraMovementSystem()
{
}

void BasicCameraMovementSystem::beginFrame()
{
	if (!m_cameraToControl)
		return;

	// Filter movable
	if (!m_cameraToControl->hasComponents(COMPONENT_BASIC_CAMERA_MOVEMENT, COMPONENT_INPUT, COMPONENT_CAMERA))
		return;

	CameraComponent& cameraComponent = m_cameraToControl->camera;
	BasicCameraMovementComponent& movementVars = m_cameraToControl->basicCameraMovement;
	InputComponent& input = m_cameraToControl->input;

	glm::mat3 coordinateSystem;
	glm::vec3 pos = cameraComponent.getPosition();
	glm::vec3& right = coordinateSystem[0] = cameraComponent.getRight();
	glm::vec3 up = coordinateSystem[1] = cameraComponent.getUp();
	glm::vec3& forward = coordinateSystem[2] = cameraComponent.getForward();

	float moveSpeed = movementVars.moveSpeed;
	float orientationSensitivity = movementVars.orientationSensitivity;

	float deltaAzimuth = -orientationSensitivity * input.orientationDelta.x;
	float deltaElevation = -orientationSensitivity * input.orientationDelta.y;
	float roll = -orientationSensitivity * input.orientationDelta.z;

	// Displacement
	glm::vec3 axes = GLMUtils::limitVec(input.axis, 1);
	axes = coordinateSystem * axes; // Convert movement to local coordinates
	pos += moveSpeed * axes;

	// Rotation
	// Prevent elevation going past 90 degrees
	glm::mat3 elevationMat{ 1 };
	float elevation = glm::pi<float>() / 2 - std::acos(glm::dot(forward, up));
	if (std::abs(elevation + deltaElevation) < glm::pi<float>() / 2)
		elevationMat = glm::rotate(deltaElevation, right);
	glm::mat3 azimuthMat = glm::rotate(deltaAzimuth, up);
	glm::mat3 rollMat = glm::rotate(roll, forward);

	coordinateSystem = rollMat * azimuthMat * elevationMat * coordinateSystem;
	cameraComponent.setLookAt(pos, pos + forward, up);
}

void BasicCameraMovementSystem::update(Entity& entity)
{
	if (entity.hasComponents(COMPONENT_BASIC_CAMERA_MOVEMENT, COMPONENT_CAMERA) && !m_cameraToControl)
		setCameraToControl(&entity);
}

void BasicCameraMovementSystem::endFrame()
{
}

void BasicCameraMovementSystem::setCameraToControl(Entity* entity)
{
	if (entity && !entity->hasComponents(COMPONENT_CAMERA))
		return;

	m_cameraToControl = entity;

	if (m_cameraToControl) {
		if (!m_cameraToControl->hasComponents(COMPONENT_BASIC_CAMERA_MOVEMENT)) {
			m_cameraToControl->addComponents(COMPONENT_BASIC_CAMERA_MOVEMENT);
			m_cameraToControl->basicCameraMovement.moveSpeed = 0.2f;
			m_cameraToControl->basicCameraMovement.orientationSensitivity = 0.01f;
		}
		if (!m_cameraToControl->hasComponents(COMPONENT_INPUT_MAP))
			setDefaultMouseControls(*m_cameraToControl);
	}
}

void BasicCameraMovementSystem::setDefaultMouseControls(Entity& entity)
{
	entity.addComponents(COMPONENT_INPUT_MAP);
	entity.addComponents(COMPONENT_INPUT);

	entity.input = {};
	entity.inputMap = {};
	entity.inputMap.mouseInputEnabled = true;
	entity.inputMap.leftBtnMap = GLFW_KEY_A;
	entity.inputMap.rightBtnMap = GLFW_KEY_D;
	entity.inputMap.forwardBtnMap = GLFW_KEY_W;
	entity.inputMap.backwardBtnMap = GLFW_KEY_S;
	entity.inputMap.downBtnMap = GLFW_KEY_Q;
	entity.inputMap.upBtnMap = GLFW_KEY_E;
}
