#pragma once

#include <glad\glad.h>

struct FrameBuffer {
	FrameBuffer();

	GLuint id;
	GLenum target;
};