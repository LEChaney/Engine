#version 420 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inColor;

out VertexData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
	vec3 color;
	vec4 lightSpacePos;
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

layout(std140, binding = 1) uniform LightData {
	vec4 directionalLightDirections[4];
	vec4 directionalLightColors[4];
	mat4 lightSpaceMatrix;
	uint numDirectionalLights;
};

void main()
{
	vec3 worldPos = (u.model * vec4(inPosition, 1)).xyz;

    o.normal = (u.model * vec4(inNormal, 0)).xyz; // TODO: Do inverse transpose
    o.texCoord = inTexCoord;
	o.viewDir = (u.cameraPos.xyz - worldPos).xyz;
	o.worldPos = worldPos;
	o.color = inColor;
	o.lightSpacePos = lightSpaceMatrix * vec4(worldPos, 1.0);

    gl_Position = u.projection * u.view * vec4(worldPos, 1);
}
