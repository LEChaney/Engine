#pragma once

class Entity;

struct TerrainFollowComponent {
	TerrainFollowComponent();

	Entity* terrainToFollow;
	float followerHalfHeight;
	bool jumpEnabled;
};