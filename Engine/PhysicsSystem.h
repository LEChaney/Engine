#pragma once

#include "Entity.h"
#include "System.h"

class Scene;

class PhysicsSystem : public System {
public:
	PhysicsSystem(Scene&);

	void update() override;

	void beginFrame() override {};
	void endFrame() override {};
};