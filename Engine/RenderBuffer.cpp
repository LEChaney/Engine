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

#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(GLenum format, GLsizei width, GLsizei height)
	: target{ GL_RENDERBUFFER }
{
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(target, id);
	glRenderbufferStorage(target, format, width, height);

	glBindRenderbuffer(target, 0);
}
