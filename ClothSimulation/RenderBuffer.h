#pragma once

#include <glad\glad.h>

struct RenderBuffer {
	RenderBuffer() = default;
	RenderBuffer(GLenum format, GLsizei width, GLsizei height);

	GLuint id;
	GLenum target;
};