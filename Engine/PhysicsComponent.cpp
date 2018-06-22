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

#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent()
	: velocity{ 0, 0, 0 }
	, acceleration{ 0, 0, 0 }
	, gravityEnabled{ true }
{
}
