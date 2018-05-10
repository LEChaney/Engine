#pragma once

#include "Screen.h"
#include <vector>
class Entity;
class GameplayScreen : public Screen
{
public:
	GameplayScreen();
	~GameplayScreen() override;
private:
	std::vector<Entity*> m_playerList;
};

