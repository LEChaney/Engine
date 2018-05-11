#pragma once

#include "Scene.h"
#include "System.h"

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
	Screen() {};

	Scene m_scene;
	std::vector<std::unique_ptr<System>> m_activeSystems;
};

