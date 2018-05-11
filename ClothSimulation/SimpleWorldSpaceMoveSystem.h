#pragma once

#include "System.h"

class Scene;

class SimpleWorldSpaceMoveSystem : public System {
public:
	SimpleWorldSpaceMoveSystem(Scene&);

	// Inherited via System
	virtual void update(Entity&) override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};