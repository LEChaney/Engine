#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent()
	: velocity{ 0, 0, 0 }
	, acceleration{ 0, 0, 0 }
	, gravityEnabled{ true }
{
}
