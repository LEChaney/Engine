#pragma once

#include "System.h"

#include <glad\glad.h>

class Scene;
struct ClothComponent;
struct PointMass;

class ClothSystem : public System {
public:
	ClothSystem(Scene&);
	~ClothSystem() override;
	ClothSystem(const ClothSystem&) = delete;
	ClothSystem& operator=(const ClothSystem&) = delete;

	static Entity& createCloth(Scene&, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea);

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	const GLuint m_kNumConstraintSolverIterations;
	const GLfloat m_kDamping;
	const GLfloat m_kTimeStep;
	const GLfloat m_kTimeStepSq;
};

namespace Prefabs {
	Entity& createCloth(Scene&, GLuint numPointsX, GLuint numPointsY, GLfloat width, GLfloat height, GLfloat weightPerUnitArea);
}