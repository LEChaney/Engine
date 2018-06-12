#pragma once

#include "System.h"

#include <glad\glad.h>

class Scene;

class ClothSystem : public System {
public:
	ClothSystem(Scene&);
	~ClothSystem() override;
	ClothSystem(const ClothSystem&) = delete;
	ClothSystem& operator=(const ClothSystem&) = delete;

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

	glm::vec3 windDirection;
	float windForce;

private:
	const GLuint m_kNumConstraintSolverIterations;
	const GLfloat m_kDamping;
	const GLfloat m_kTimeStep;
	const GLfloat m_kTimeStepSq;
	const GLfloat m_kBurnPercentPerSecond;
};
