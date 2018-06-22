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

struct CameraDataBlockFormat {
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec4 cameraPos;
};

struct ModelDataBlockFormat {
	glm::mat4 model;
};