#pragma once

#include "System.h"

class Scene;

class TerrainFollowSystem : public System {
public:
	TerrainFollowSystem(Scene&);

	// Inherited via System
	virtual void update(Entity &) override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};