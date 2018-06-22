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

#include <glad\glad.h>

struct RenderBuffer {
	RenderBuffer() = default;
	RenderBuffer(GLenum format, GLsizei width, GLsizei height);

	GLuint id;
	GLenum target;
};