#pragma once

#include <glm\glm.hpp>

struct CameraDataBlockFormat {
	glm::mat4 view;
	glm::mat4 projection;
};

struct ModelDataBlockFormat {
	glm::mat4 world;
};