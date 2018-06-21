#include "TerrainFollowComponent.h"

TerrainFollowComponent::TerrainFollowComponent()
	: terrainToFollow{ nullptr }
	, followerHalfHeight{ 1.0f }
	, jumpEnabled{ false }
{
}
