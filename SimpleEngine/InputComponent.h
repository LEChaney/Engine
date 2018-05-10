//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for receving input.
//                Input mappings are also set in this component.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include <array>

struct InputComponent {
	// Vehicle inputs
	float turnAxis;
	bool acceleratorDown;
	bool brakeDown;

	// Direct camera control inputs
	glm::vec3 orientationDelta;
	glm::vec3 axis; // Allows movement is all three axes
};