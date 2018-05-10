#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>

class Entity;
class Scene;

struct TerrainComponent {
	std::vector<float> heightMap;
	glm::ivec2 heightMapDimensions;
	float heightScale;
	float size;
};

namespace TerrainUtils {
	// Returns true if casting position to terrain height succeeded (position is above terrain)
	// The terrain height is output in the outHeight parameter.
	bool castPosToTerrainHeight(const Entity& terrainEntity, const glm::vec3& pos, float& outHeight);

	// Returns true if casting position to heightmap texture coordinate succeeded (position is above terrain).
	// The texture coordinate is output the outTexCoord parameter
	bool castPosToHeightMapTexCoord(const Entity& terrainEntity, const glm::vec3& entityPos, glm::vec2& outTexCoord);
}

namespace Prefabs {
	Entity& createTerrain(Scene& scene, const std::string& heightMapFile, float size, GLsizei baseTessellation = 50, const glm::vec3& position = { 0, 0, 0 });
}