#include "ClothComponent.h"

PointMass& ClothComponent::getPointMass(GLuint row, GLuint col)
{
	return pointMasses.at(row * numPointMassesX + col);
}
