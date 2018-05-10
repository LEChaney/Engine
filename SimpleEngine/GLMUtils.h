//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of utility functions that rely on the GLM libary
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "TransformComponent.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\euler_angles.hpp>

namespace GLMUtils {
	// Limits a vector to the specified magnitude
	template <typename VecT>
	VecT limitVec(const VecT& vec, float maxMagnitude)
	{
		auto magnitude = glm::length(vec);
		if (magnitude > maxMagnitude) {
			return glm::normalize(vec) * maxMagnitude;
		}
		return vec;
	}

	inline glm::mat4 eulerToMat(const glm::vec3& eulerAngles) 
	{
		return glm::yawPitchRoll(eulerAngles.y, eulerAngles.x, eulerAngles.z);
	}

	inline glm::mat4 transformToMat(const TransformComponent& transform)
	{
		return glm::translate(glm::mat4{}, transform.position)
		     * eulerToMat(transform.eulerAngles)
		     * glm::scale(glm::mat4{}, transform.scale);
	}
}
