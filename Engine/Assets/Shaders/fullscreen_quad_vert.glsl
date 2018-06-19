#version 420 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec2 texCoord;

void main()
{
    texCoord = inTexCoord;
    gl_Position = vec4(inPosition.xy, 0.0, 1.0); 
}  