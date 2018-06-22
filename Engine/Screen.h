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

#include "Scene.h"
#include "System.h"

#include <nanogui/nanogui.h>

#include <vector>
#include <memory>

class Screen
{
public:
	virtual ~Screen() {};
	Screen(const Screen&) = delete;
	Screen& operator=(const Screen&) = delete;

	void update();

protected:
	Screen();

	Scene m_scene;
	std::vector<std::unique_ptr<System>> m_activeSystems;
	nanogui::ref<nanogui::Screen> m_uiScreen;
};

