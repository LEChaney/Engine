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

#include "Texture.h"

Texture Texture::Texture2D(GLsizei width, GLsizei height, GLint internalFormat, GLenum componentType, GLenum format, const void* data)
{
	if (format == -1)
		format = internalFormat;

	Texture texture;
	texture.target = GL_TEXTURE_2D;
	texture.width = width;
	texture.height = height;

	glGenTextures(1, &texture.id);
	glBindTexture(texture.target, texture.id);
	glTexImage2D(texture.target, 0, internalFormat, width, height, 0, format, componentType, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(texture.target, 0);

	return texture;
}
