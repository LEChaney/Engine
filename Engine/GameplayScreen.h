//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

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

