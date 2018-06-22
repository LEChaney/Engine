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

#include "System.h"

class Scene;
struct PointMass;

class ClothPickingSystem : public System {
public:
	ClothPickingSystem(Scene&, Entity& cameraEntity);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	PointMass* mousePick();

	void updateGrabbedPointMass();

	Entity* m_cameraEntity;
	int m_prevLeftMouseState;
	int m_prevRightMouseState;
	PointMass* m_grabbedPointMass;
	glm::dvec2 m_mousePos;
	glm::dvec2 m_prevMousePos;
};