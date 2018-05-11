//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A header only collection of ai movement functions.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#define _USE_MATH_DEFINES

#include "AIUtils.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "Scene.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

// Returns a force to move towards the target position.
glm::vec3 seek(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{

	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);
	glm::vec3 desiredVelocity;
	desiredVelocity = displacement / displacementMag * maxMoveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	//RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity) * 100, { 0, 0, 1 });

	return steering;
}

// Returns a force to move towards the target position, slows down with arrival when close.
glm::vec3 seekWithArrival(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);

	// Apply arrival behaviour as the entity gets close to the target.
	glm::vec3 desiredVelocity;
	if (displacementMag < 2)
		desiredVelocity = displacement / displacementMag * maxMoveSpeed * (displacementMag / 2);
	else
		desiredVelocity = displacement / displacementMag * maxMoveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	//RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity), { 1.0f, 1.0f, 0.0f });
	//RenderSystem::drawDebugArrow(currentPosition, steering, glm::length(steering), { 1.0f, 1.0f, 1.0f });

	return steering;
}

// Returns an acceleration to move to a move to pursue a target
glm::vec3 pursue(glm::vec3 targetPosition, glm::vec3 targetsVelocity, float targetsMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	float T = glm::length(glm::vec2{ targetPosition.x - currentPosition.x, targetPosition.z - currentPosition.z }) / targetsMoveSpeed;
	glm::vec3 futurePosition = targetPosition + targetsVelocity * T;

	futurePosition.y = currentPosition.y;

	return seek(futurePosition, currentPosition, currentVelocity, maxMoveSpeed);
}

glm::vec3 followLeader(glm::vec3 targetPosition, glm::vec3 targetVelocity, glm::vec3 targetPreviousVelocity, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	// Generate a vector in the opposite direction the target is heading.
	glm::vec2 tv;
	if (targetVelocity != glm::vec3{ 0,0,0 })
		tv = glm::vec2{ targetVelocity.x * -1.0f , targetVelocity.z * -1.0f };
	else if (targetPreviousVelocity != glm::vec3{ 0,0,0 })
		tv = glm::vec2{ targetPreviousVelocity.x * -1.0f , targetPreviousVelocity.z * -1.0f };
	else
		tv = glm::vec2{ 0.0f , -1.0f };
	// Change its length to be equal to the desired follow distance.
	tv = (tv / glm::length(tv)) * 2.0f;
	// Find the position to follow.
	glm::vec3 followPosition = glm::vec3{ targetPosition.x + tv.x, targetPosition.y, targetPosition.z + tv.y };

	//Seek to that point
	return seekWithArrival(followPosition, currentPosition, currentVelocity, maxMoveSpeed);
}

void steer(Entity& entity, const glm::vec3& steeringAcceleration)
{
	// Weight the acceleration according to entities acceleration weight
	entity.physics.acceleration = steeringAcceleration * entity.vehicleMovement.accelerationWeight;

	// Limit the steering acceleration.
	entity.physics.acceleration = GLMUtils::limitVec<glm::vec3>(entity.physics.acceleration, entity.vehicleMovement.maxAcceleration);
}
