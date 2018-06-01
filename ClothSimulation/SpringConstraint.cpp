#include "SpringConstraint.h"

#include <glm/glm.hpp>

using namespace glm;

SpringConstraint::SpringConstraint(PointMass& p1, PointMass& p2, GLfloat stiffness)
	: m_p1{ p1 }
	, m_p2{ p2 }
	, m_stiffness{ stiffness }
{
	m_restLength = glm::length(p2.position - p1.position);
}

void SpringConstraint::solveConstraint()
{
	vec3 p1ToP2 = m_p2.position - m_p1.position;
	float currentLength = glm::length(p1ToP2);
	vec3 correctionVector = p1ToP2 * (1 - currentLength / m_restLength);
	vec3 halfCorrectionVector = 0.5f * correctionVector;
	m_p1.position += correctionVector;
	m_p2.position -= correctionVector;
}
