#version 420 core

out vec4 outColor;
  
in vec2 texCoord;

uniform float numSamples = 5;
const float gamma = 2.2;

uniform mat4 inverseViewProjection;
uniform mat4 previousViewProjection;

uniform sampler2D sceneSampler;
uniform sampler2D depthSampler;

void main()
{
	// Get the depth buffer value at this pixel
	float depth = texture(depthSampler, texCoord).r;

	// Convert texel position to world space
	vec4 viewportPos = vec4(texCoord.x * 2 - 1, texCoord.y * 2 - 1, depth * 2 - 1, 1);
	vec4 worldPos = inverseViewProjection * viewportPos;
	worldPos /= worldPos.w;

	// Get the previous view port position of the texel
	vec4 prevViewportPos = previousViewProjection * worldPos;
	prevViewportPos /= prevViewportPos.w;

	// Calc texel velocity
	vec2 velocity = (viewportPos.xy - prevViewportPos.xy) / 4.0f; // Scale velocity so blur isn't so jarring

	// Get the initial color at this texel
	vec4 color = texture(sceneSampler, texCoord);

	// Sum color along velocity vector
	vec2 sampleCoord = texCoord + velocity;
	for (int i = 1; i < numSamples; ++i, sampleCoord += velocity) {
		color += texture(sceneSampler, sampleCoord);
	}

	// Average color to get final output
	vec4 bluredColor = color / numSamples;



	// Apply gamma correction
	outColor = vec4(pow(bluredColor.rgb, vec3(1 / gamma)), bluredColor.a);
}