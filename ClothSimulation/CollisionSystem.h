#pragma once

#include "System.h"

#include <glad\glad.h>

class Scene;

class CollisionSystem : public System {
public:
	CollisionSystem(Scene&);
	~CollisionSystem() override;
	CollisionSystem(const CollisionSystem&) = delete;
	CollisionSystem& operator=(const CollisionSystem&) = delete;

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	void SphereCollision(Entity& clothEntity, Entity& sphereEntity);
};