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

#pragma once

class Entity;

struct TerrainFollowComponent {
	TerrainFollowComponent();

	Entity* terrainToFollow;
	float followerHalfHeight;
	bool jumpEnabled;
	bool isOnGround;
};