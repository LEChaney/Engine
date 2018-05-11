#pragma once

#include "System.h"

class BasicCameraMovementSystem : public System {
public:
	BasicCameraMovementSystem(Scene& scene);
	~BasicCameraMovementSystem() override;

	// Inherited via System
	virtual void update(Entity&) override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

	void setCameraToControl(Entity*);
	void setDefaultMouseControls(Entity&);

private:
	Entity* m_cameraToControl;
};
