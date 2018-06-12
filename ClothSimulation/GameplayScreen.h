#pragma once

#include "Screen.h"

#include <vector>

class Entity;
class ClothSystem;

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
	int m_hookDistance;
	ClothSystem* m_clothSystem;
	float m_clothSize;
	// Here to be eddited in all cloth resetting lambdas
	nanogui::Slider *m_foldSlider;
	nanogui::TextBox *m_foldTextBox;
};

