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

void main()
{
	// Take sample from all offsets
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = textureOffset(sceneSampler, texCoord, offsets[i]).rgb;
    }

	// Convolve samples with kernels
    vec3 gradX = vec3(0);
	vec3 gradY = vec3(0);
    for(int i = 0; i < 9; ++i) {
        gradX += sampleTex[i] * kernelX[i];
		gradY += sampleTex[i] * kernelY[i];
	}
	vec3 grad = sqrt(gradX * gradX + gradY * gradY);
    
    outColor = vec4(grad, 1.0);
}