#pragma once

class Entity;

struct TerrainFollowComponent {
	Entity* terrainToFollow;
	float followerHalfHeight;
};