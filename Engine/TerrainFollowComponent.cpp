//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
//                scene.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "TerrainFollowComponent.h"

TerrainFollowComponent::TerrainFollowComponent()
	: terrainToFollow{ nullptr }
	, followerHalfHeight{ 1.0f }
	, jumpEnabled{ false }
	, isOnGround{ false }
{
}
