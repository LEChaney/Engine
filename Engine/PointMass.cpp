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

#include "PointMass.h"

PointMass::PointMass(glm::vec3 position, GLfloat mass, GLboolean isFixed)
	: m_position{ position }
	, prevPosition{ position }
	, force{ 0, 0, 0 }
	, mass{ mass }
	, isFixed{ isFixed }
	, burnedPercent{ 0 }
	, isBurning{ false }
{
}

const glm::vec3& PointMass::getPosition()
{
	return m_position;
}

void PointMass::setPosition(const glm::vec3 & newPosition)
{
	m_position = newPosition;
}

void PointMass::addOffset(const glm::vec3& offset)
{
	if (!isFixed)
		m_position += offset;
}
