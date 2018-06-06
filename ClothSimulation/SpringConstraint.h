#pragma once

#include "PointMass.h"

#include <glad\glad.h>

// SpringConstraint struct
class SpringConstraint {
public:
	// Constructor
	// The break distance is the maximum distance the constaint can be away from the rest length before it breaks
	SpringConstraint(PointMass& p1, PointMass& p2, GLfloat stiffness, GLfloat breakDistance);

	// Solves the constraint by moving the pointmasses involved to their desired position.
	// Should break informs the system whether the constraint is broken and should be removed.
	void solveConstraint();

	PointMass& getPointMass1();
	const PointMass& getPointMass1() const;
	PointMass& getPointMass2();
	const PointMass& getPointMass2() const;

	bool isBroken() const;

private:
	PointMass* m_p1;
	PointMass* m_p2;
	GLfloat m_restLength;
	GLfloat m_stiffness; // Has range [0, 1], where 0 is completely free and 1 is a completely rigid constraint
	GLfloat m_breakDistance;
	bool m_isBroken;
};