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
	void setupGui();
	std::vector<Entity*> m_playerList;
	nanogui::Window* window;
	int m_clothPointsX;
	int m_clothPointsY;
};

