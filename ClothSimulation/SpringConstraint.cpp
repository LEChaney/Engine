#include "SpringConstraint.h"

#include <glm/glm.hpp>

using namespace glm;

SpringConstraint::SpringConstraint(PointMass& p1, PointMass& p2, GLfloat stiffness)
	: m_p1{ p1 }
	, m_p2{ p2 }
	, m_stiffness{ stiffness }
{
	m_restLength = glm::length(p2.getPosition() - p1.getPosition());
}

void SpringConstraint::solveConstraint()
{
	vec3 p1ToP2 = m_p2.getPosition() - m_p1.getPosition();
	float currentLength = glm::length(p1ToP2);
	vec3 correctionVector = p1ToP2 * (1 - m_restLength / currentLength);

	float invMass1 = 1 / m_p1.mass;
	float invMass2 = 1 / m_p2.mass;
	//                        vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv = 0.5 when masses are equal
	float correctionWeight1 = (invMass1 / (invMass1 + invMass2)) * m_stiffness;
	float correctionWeight2 = m_stiffness - correctionWeight1;
	
	m_p1.addOffset(correctionWeight1 * correctionVector);
	m_p2.addOffset(-correctionWeight2 * correctionVector);
}
