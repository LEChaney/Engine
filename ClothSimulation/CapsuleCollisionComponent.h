#pragma once
#include <glm\glm.hpp>
#include "TransformComponent.h"

struct CapsuleCollisionComponent
{
	CapsuleCollisionComponent();

	glm::vec3 getCapsuleEnd(TransformComponent& capsuleTransform);
	float getRadius(TransformComponent& capsuleTransform);
	
private:
	float radius;
	glm::vec3 capsuleEnd;
};