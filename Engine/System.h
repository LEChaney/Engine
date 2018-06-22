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

#include "Entity.h"
#include "Scene.h"

class System {
public:
	System(Scene& scene);
	System(const System&) = delete;
	System& operator=(const System&) = delete;

	virtual void update() = 0;
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

	virtual ~System() {};

protected:
	Scene& m_scene;
};