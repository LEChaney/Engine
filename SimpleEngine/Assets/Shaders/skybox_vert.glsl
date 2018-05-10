#version 420 core

layout (location = 0) in vec3 inPosition;

out VertexData {
    vec3 textureDir;
} o;

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

void main()
{
	mat4 view = u.view;
	view[3] = vec4(0, 0, 0, 1);
	o.textureDir = inPosition;
	gl_Position = (u.projection * view * vec4(inPosition, 1.0)).xyww;
}
