//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

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