#pragma once

#include "System.h"

class Scene;
struct PointMass;

class MousePickingSystem : public System {
public:
	MousePickingSystem(Scene&, Entity& cameraEntity);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	PointMass* mousePick();

	void updateGrabbedPointMass();

	Entity* m_cameraEntity;
	int m_prevMouseState;
	PointMass* m_grabbedPointMass;
	glm::dvec2 m_mousePos;
	glm::dvec2 m_prevMousePos;
};