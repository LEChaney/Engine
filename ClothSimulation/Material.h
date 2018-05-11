//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for storing per entity
//                material information used by the GPU.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "ShaderParams.h"
#include "Texture.h"
#include "Shader.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <string>

struct Material {
	const Shader* shader;
	ShaderParams shaderParams;
	std::vector<Texture> colorMaps;
	std::vector<Texture> metallicnessMaps;
	std::vector<Texture> shininessMaps;
	std::vector<Texture> normalMaps;
	std::vector<Texture> heightMaps;
	std::vector<Texture> displacementMaps;
	bool willDrawDepth;
	glm::vec3 debugColor;
	float heightMapScale;
};