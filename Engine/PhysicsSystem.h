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
#include "System.h"

class Scene;

class PhysicsSystem : public System {
public:
	PhysicsSystem(Scene&);

	void update() override;

	void beginFrame() override {};
	void endFrame() override {};
};