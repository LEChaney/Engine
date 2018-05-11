#pragma once

#include "System.h"

#include <vector>

class Entity;
class Scene;

class CameraSystem : public System
{
public:
	CameraSystem(Scene&, std::vector<Entity*>& playerList);
	~CameraSystem();

	// Inherited via System
	virtual void update(Entity&) override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	std::vector<Entity*>& m_playerList;
};

