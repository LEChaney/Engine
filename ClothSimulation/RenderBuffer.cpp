#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(GLenum format, GLsizei width, GLsizei height)
	: target{ GL_RENDERBUFFER }
{
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(target, id);
	glRenderbufferStorage(target, format, width, height);

	glBindRenderbuffer(target, 0);
}
