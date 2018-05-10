#include "Texture.h"

Texture Texture::Texture2D(GLsizei width, GLsizei height, GLenum colorFormat, GLenum componentType, const void* data)
{
	Texture texture;
	texture.target = GL_TEXTURE_2D;
	glGenTextures(1, &texture.id);
	glBindTexture(texture.target, texture.id);
	glTexImage2D(texture.target, 0, colorFormat, width, height, 0, colorFormat, componentType, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(texture.target, 0);

	return texture;
}
