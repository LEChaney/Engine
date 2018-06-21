#pragma once

#include "System.h"

class Scene;

class ParticleSystem : public System
{
public:
	ParticleSystem(Scene&);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};

