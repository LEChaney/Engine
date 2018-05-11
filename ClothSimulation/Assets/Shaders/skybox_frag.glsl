#version 420 core

in VertexData {
    vec3 textureDir;
} i;

out vec4 outColor;

layout (std140) uniform UniformBlock {
    mat4 model;
    mat4 view;
    mat4 projection;
	vec4 cameraPos;
	vec4 spotlightPositions[8];
	vec4 spotlightDirections[8];
	vec4 spotlightColors[8];
	uint numSpotlights;
	float metallicness;
	float glossiness;
	float specBias;
	float time;
	bool discardTransparent;
} u;

uniform samplerCube texSampler0;

void main(void)
{
    outColor = texture(texSampler0, i.textureDir);

	if (u.discardTransparent && outColor.a < 0.5f)
		discard;
}
