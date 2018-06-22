#version 420 core

const int MAX_JOINTS = 100; //max joints allowed in a skeleton
const int MAX_WEIGHTS = 4; //max number of joints that can affect a vertex

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

out VertexData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
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

uniform mat4 jointTransforms[MAX_JOINTS];

void main()
{
	vec4 totalLocalPosition = vec4(0.0);
	vec4 totalNormal = vec4(0.0);

	for(int i = 0; i < MAX_WEIGHTS; i++) {
		vec4 posePosition = jointTransforms[boneIds[i]] * vec4(inPosition, 1.0);
		totalLocalPosition += posePosition * weights[i];
		vec4 worldNormal = jointTransforms[boneIds[i]] * vec4(inNormal, 0.0);
		totalNormal += worldNormal * weights[i];
	} 

	o.texCoord = inTexCoord;
	o.worldPos = vec3(u.model * totalLocalPosition);
	o.normal = mat3(transpose(inverse(u.model))) * vec3(totalNormal);
	o.lightSpacePos = lightSpaceMatrix * vec4(o.worldPos, 1.0);
	o.viewDir = (u.cameraPos.xyz - o.worldPos).xyz;

	gl_Position = u.projection * u.view * vec4(o.worldPos, 1.0);
}
