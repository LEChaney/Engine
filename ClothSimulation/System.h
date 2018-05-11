#pragma once

#include "Entity.h"
#include "Scene.h"

class System {
public:
	System(Scene& scene);
	System(const System&) = delete;
	System& operator=(const System&) = delete;

	virtual void update(Entity&) = 0;
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

	virtual ~System() {};

protected:
	Scene& m_scene;
};