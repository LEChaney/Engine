#version 420 core

out vec4 outColor;
  
in vec2 texCoord;

uniform sampler2D sceneSampler;

const float kernelX[9] = float[](
    1,  0, -1,
    2,  0, -2,
    1,  0, -1
);

const float kernelY[9] = float[](
    1,  2,  1,
    0,  0,  0,
   -1, -2, -1
);

const ivec2 offsets[9] = ivec2[](
    ivec2(-1,  1), // top-left
    ivec2( 0,  1), // top-center
    ivec2( 1,  1), // top-right
    ivec2(-1,  0), // center-left
    ivec2( 0,  0), // center-center
    ivec2( 1,  0), // center-right
    ivec2(-1, -1), // bottom-left
    ivec2( 0, -1), // bottom-center
    ivec2( 1, -1)  // bottom-right
);

const float gamma = 2.2;

void main()
{
	// Take sample from all offsets
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = textureOffset(sceneSampler, texCoord, offsets[i]).rgb;
    }

	// Convolve samples with kernels to get gradient
    vec3 gradX = vec3(0);
	vec3 gradY = vec3(0);
    for(int i = 0; i < 9; ++i) {
        gradX += sampleTex[i] * kernelX[i];
		gradY += sampleTex[i] * kernelY[i];
	}
	vec3 grad = sqrt(gradX * gradX + gradY * gradY);
	float gradIntensity = 0.2126 * grad.r + 0.7152 * grad.g + 0.0722 * grad.b;

	// Get the original color of this texel
	vec3 color = texture(sceneSampler, texCoord).rgb;
	float intensity = length(color);

	// Discretize the intensity, based on a few cutoff points
    if (intensity > 0.95)
        color = vec3(1, 1, 1) * normalize(color);
    else if (intensity > 0.5)
        color = vec3(0.7, 0.7, 0.7) * normalize(color);
    else if (intensity > 0.05)
        color = vec3(0.35, 0.35, 0.35) * normalize(color);
    else
        color = vec3(0.1, 0.1, 0.1) * normalize(color);
    
	// Gamma correct and add outline
    outColor = vec4(pow(color, vec3(1 / gamma)) - gradIntensity, 1.0);
}