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

struct Texture {
	static Texture Texture2D(GLsizei width, GLsizei height, GLint internalFormat = GL_RGB, GLenum componentType = GL_UNSIGNED_BYTE, GLenum format = -1, const void* data = nullptr);

	GLuint id;
	GLenum target;
	GLsizei width;
	GLsizei height;
};