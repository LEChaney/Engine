#include "SpringConstraint.h"

#include <glm/glm.hpp>

using namespace glm;

SpringConstraint::SpringConstraint(PointMass& p1, PointMass& p2, GLfloat stiffness, GLfloat breakDistance)
	: m_p1{ &p1 }
	, m_p2{ &p2 }
	, m_stiffness{ stiffness }
	, m_breakDistance{ breakDistance }
	, m_isBroken{ false }
{
	m_restLength = glm::length(p2.getPosition() - p1.getPosition());
}

void SpringConstraint::solveConstraint()
{
	if (m_isBroken) {
		return;
	}

	vec3 p1ToP2 = m_p2->getPosition() - m_p1->getPosition();
	float currentLength = glm::length(p1ToP2);
	vec3 correctionVector = p1ToP2 * (1 - m_restLength / currentLength);

	if (glm::length(correctionVector) >= m_breakDistance) {
		m_isBroken = true;
		return;
	}

	float invMass1 = 1 / m_p1->mass;
	float invMass2 = 1 / m_p2->mass;
	//                        vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv = 0.5 when masses are equal
	float correctionWeight1 = (invMass1 / (invMass1 + invMass2)) * m_stiffness;
	float correctionWeight2 = m_stiffness - correctionWeight1;
	
	m_p1->addOffset(correctionWeight1 * correctionVector);
	m_p2->addOffset(-correctionWeight2 * correctionVector);
}

PointMass& SpringConstraint::getPointMass1()
{
	return *m_p1;
}

const PointMass& SpringConstraint::getPointMass1() const
{
	return *m_p1;
}

PointMass& SpringConstraint::getPointMass2()
{
	return *m_p2;
}

const PointMass& SpringConstraint::getPointMass2() const
{
	return *m_p2;
}

bool SpringConstraint::isBroken() const
{
	return m_isBroken;
}
