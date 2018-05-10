//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A struct containing model, view and projection matrices
//                as well as camera position.
//                Corresponds to an equivalent uniform buffer object on the
//                GPU with same alignment.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <array>

// TODO: Split this up, and make offsets easy to access, so we can easily:
// buffer constant projection matrix once,
// View matrix, camera position and lights per frame,
// and model matrix and shader parameters per object render call.
// Could also batch render calls based on materials and only buffer separate view matrices per same object render call.
struct UniformBlockFormat {
	static const GLuint s_kMaxSpotlights = 8;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec4 cameraPos;
	std::array<glm::vec4, s_kMaxSpotlights> spotlightPositions;
	std::array<glm::vec4, s_kMaxSpotlights> spotlightDirections;
	std::array<glm::vec4, s_kMaxSpotlights> spotlightColors;
	GLuint numSpotlights;
	GLfloat metallicness;
	GLfloat glossiness;
	GLfloat specBias;
	GLfloat time;
	GLboolean discardTransparent; // 1 Byte, needs to come last to avoid issues with std140 padding.
};