#pragma once

#include <glad\glad.h>

struct Texture {
	static Texture Texture2D(GLsizei width, GLsizei height, GLenum colorFormat = GL_RGB, GLenum componentType = GL_UNSIGNED_BYTE, const void* data = nullptr);

	GLuint id;
	GLenum target;
};