#version 420 core

layout (triangles, equal_spacing, ccw) in;

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
} te_in[];

out VertexData {
	vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} te_out;

uniform sampler2D normalMapSampler;
uniform sampler2D heightMapSampler;
uniform float heightMapScale;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) 
{
	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

void main()
{
	te_out.texCoord = interpolate2D(te_in[0].texCoord, te_in[1].texCoord, te_in[2].texCoord);
	te_out.normal = texture(normalMapSampler, te_out.texCoord).xyz;
	te_out.worldPos = interpolate3D(te_in[0].worldPos, te_in[1].worldPos, te_in[2].worldPos);
	te_out.worldPos.y += texture(heightMapSampler, te_out.texCoord).r * heightMapScale;
	te_out.viewDir = normalize(u.cameraPos.xyz - te_out.worldPos);

	gl_Position = u.projection * u.view * vec4(te_out.worldPos, 1);
}