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

class BasicCameraMovementSystem : public System {
public:
	BasicCameraMovementSystem(Scene& scene);
	~BasicCameraMovementSystem() override;

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

	void setCameraToControl(Entity*);
	void setDefaultMouseControls(Entity&);

private:
	Entity* m_cameraToControl;
};
