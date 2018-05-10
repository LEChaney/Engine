#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

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
} sh;

in VertexData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} gs_in[];

out VertexData {
	vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} gs_out;

const vec3 windDir = vec3(0, 0, -1);
const float windMagnitude = 0.75f;
const float snapDistance = 0.1f;

uniform sampler2D texSampler1;

float random(vec2 st) {
	return fract(sin(dot(st.xy,
		vec2(12.9898, 78.233)))*
		43758.5453123);
}

void main() {
	vec2 triCoord = (gs_in[0].texCoord + gs_in[1].texCoord + gs_in[2].texCoord) / 3.0f;
	float vegetationChance = texture(texSampler1, triCoord).r;

	vec3 triCenter = vec3(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		triCenter += gs_in[i].worldPos;
	}
	triCenter /= 3;
	
	// Snap to grid
	vec3 grassBase = floor(triCenter / snapDistance) * snapDistance;

	if (random(grassBase.xy) <= vegetationChance) {
		vec3 u = normalize(gs_in[1].worldPos - gs_in[0].worldPos);
		vec3 v = gs_in[2].worldPos - gs_in[0].worldPos;
		vec3 n = normalize(cross(u, v));
		v = cross(u, n);
		mat3 tangentSpace = mat3(u, n, v);

		const mat4 vp = sh.projection * sh.view;

		for (int i = 0; i < 2; ++i) {
			// Wind
			vec3 lowerLeft = grassBase - 0.5f * tangentSpace[0];
			vec3 lowerRight = grassBase + 0.5f * tangentSpace[0];
			vec3 upperLeft = grassBase - 0.5f * tangentSpace[0] + vec3(0, 1.0f, 0);
			vec3 upperRight = grassBase + 0.5f * tangentSpace[0] + vec3(0, 1.0f, 0);
			float leftWindCoord = -dot(upperLeft, windDir);
			float rightWindCoord = -dot(upperRight, windDir);
			vec3 upperLeftOffset = windDir * windMagnitude * (sin(leftWindCoord + sh.time) + 1);
			vec3 upperRightOffset = windDir * windMagnitude * (sin(rightWindCoord + sh.time) + 1);
			float leftBendability = dot(n, normalize(upperLeft + upperLeftOffset - lowerLeft));
			float rightBendability = dot(n, normalize(upperRight + upperRightOffset - lowerRight));
			upperLeft += upperLeftOffset * leftBendability;
			upperRight += upperRightOffset * rightBendability;
			vec3 quadNormal = cross(lowerRight - lowerLeft, upperLeft - lowerLeft);

			// Lower Left
			gs_out.normal = quadNormal;
			gs_out.texCoord = vec2(0, 1);
			gs_out.worldPos = lowerLeft;
			gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
			gl_Position = vp * vec4(gs_out.worldPos, 1);
			EmitVertex();

			// Lower Right
			gs_out.normal = quadNormal;
			gs_out.texCoord = vec2(1, 1);
			gs_out.worldPos = lowerRight;
			gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
			gl_Position = vp * vec4(gs_out.worldPos, 1);
			EmitVertex();

			// Upper Left
			gs_out.normal = quadNormal;
			gs_out.texCoord = vec2(0, 0);
			gs_out.worldPos = upperLeft;
			gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
			gl_Position = vp * vec4(gs_out.worldPos, 1);
			EmitVertex();

			// Upper Right
			gs_out.normal = quadNormal;
			gs_out.texCoord = vec2(1, 0);
			gs_out.worldPos = upperRight;
			gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
			gl_Position = vp * vec4(gs_out.worldPos, 1);
			EmitVertex();

			EndPrimitive();

			// Rotate next quad 90 degrees in tangent space
			vec3 tmp = tangentSpace[0];
			tangentSpace[0] = -tangentSpace[2];
			tangentSpace[1] = tmp;
		}
	}
}
