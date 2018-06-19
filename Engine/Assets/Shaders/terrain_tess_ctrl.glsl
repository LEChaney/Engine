#version 420 core

layout (vertices = 3) out;

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

in ControlPointData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} tc_in[];

out ControlPointData {
	vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} tc_out[];

// Calculate tesselation level for an edge (defined by 2 points) based on the distance of the edge from the camera
float getTessLevel(vec3 vertA, vec3 vertB) {
	vec3 edgeCenter = (vertA + vertB) / 2;
	float distanceFromCamera = distance(u.cameraPos.xyz, edgeCenter);
	float normalizedDistance = clamp(distanceFromCamera / 250.0f, 0, 1);
	const float bucketSize = 8;
	const float maxTess = 64;
	return clamp(floor((mix(maxTess, 0, normalizedDistance) / bucketSize)) * bucketSize, 1, maxTess);
}

void main()
{
	tc_out[gl_InvocationID].normal = tc_in[gl_InvocationID].normal;
	tc_out[gl_InvocationID].texCoord = tc_in[gl_InvocationID].texCoord;
	tc_out[gl_InvocationID].viewDir = tc_in[gl_InvocationID].viewDir;
	tc_out[gl_InvocationID].worldPos = tc_in[gl_InvocationID].worldPos;

	gl_TessLevelOuter[gl_InvocationID] = getTessLevel(tc_in[(gl_InvocationID + 1) % 3].worldPos, tc_in[(gl_InvocationID + 2) % 3].worldPos);
	if(gl_InvocationID == 2)
		gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}