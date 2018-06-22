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
	glm::mat4 lightSpaceMatrix;
	GLuint numDirectionalLights;
	//GLuint numSpotlights;
};