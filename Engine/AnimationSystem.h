#pragma once

#include "System.h"

class Scene;

class AnimationSystem : public System {
public:
	AnimationSystem(Scene&);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};