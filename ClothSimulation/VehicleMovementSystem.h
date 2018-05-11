//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles vehicleMovement based on an
//                entities input state.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "System.h"

class Scene;
class Entity;

class VehicleMovementSystem : public System {
public:
	VehicleMovementSystem(Scene& scene);

	// Updates the entities position from input
	void update(Entity& entity) override;

	void beginFrame() override { };
	void endFrame() override { };
};