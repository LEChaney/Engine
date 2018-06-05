#pragma once

#include "System.h"

class Scene;

class MousePickingSystem : public System {
public:
	MousePickingSystem(Scene&);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};