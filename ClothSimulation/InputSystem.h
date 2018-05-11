//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles input propogation.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "System.h"

#include <glm\glm.hpp>

#include <functional>
#include <vector>

class Scene;
class Entity;
class IKeyObserver;

class InputSystem : public System {
public:
	InputSystem(Scene& scene);

	// Updates the entity with input
	void update(Entity&) override;

	// Does per frame input system update
	void beginFrame() override;
	void endFrame() override {};

private:
	glm::vec2 m_mouseDelta;
};