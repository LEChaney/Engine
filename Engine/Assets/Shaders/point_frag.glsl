#version 420 core

in float lifetime;
out vec4 outColor;

void main(void)
{
	outColor = vec4(0, lifetime, 0, lifetime);
}
