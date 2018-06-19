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
};

