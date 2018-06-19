#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

#include <array>

struct LightDataBlockFormat {
	static const GLuint s_kMaxDirectionalLights = 4;

	//std::array<glm::vec4, s_kMaxSpotlights> spotlightPositions;
	//std::array<glm::vec4, s_kMaxSpotlights> spotlightDirections;
	//std::array<glm::vec4, s_kMaxSpotlights> spotlightColors;
	std::array<glm::vec4, s_kMaxDirectionalLights> directionalLightDirections;
	std::array<glm::vec4, s_kMaxDirectionalLights> directionalLightColors;
	GLuint numDirectionalLights;
	//GLuint numSpotlights;
};