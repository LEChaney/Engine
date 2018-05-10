//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Material parameters used by the GPU.
//                This struct corresponds to an equivalent 
//                uniform buffer object on the GPU side with 
//                the same alignment.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>

struct ShaderParams {
	GLfloat metallicness;
	GLfloat glossiness;
	GLfloat specBias;
	GLboolean discardTransparent;
};