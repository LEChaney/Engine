#version 420 core

out vec4 outColor;
  
in vec2 texCoord;

uniform sampler2D sceneSampler;

const float gamma = 2.2;

void main()
{ 
    outColor = vec4(pow(texture(sceneSampler, texCoord).rgb, vec3(1 / gamma)), 1.0f);
}