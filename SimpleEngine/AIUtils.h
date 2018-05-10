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
glm::vec3 seek(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns an acceleration to move to pursue a target
glm::vec3 pursue(glm::vec3 targetPosition, glm::vec3 targetVelocity, float tragetMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns a force to move towards the target position, slows down with arrival when close.
glm::vec3 seekWithArrival(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Applies steering to the entity while
// keeping the entity within the level boundaries.
// Entities will not exceed their maximum acceleration.
void steer(Entity&, const glm::vec3& steeringAcceleration);