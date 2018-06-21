#version 420 core

layout (location = 0) in vec4 inPositionAndRemainingLifetime;

layout (std140, binding=3) uniform CameraData {
    mat4 view;
    mat4 projection;
};

layout (std140, binding=2) uniform ModelData {
	mat4 model;
};

out float lifetime;

void main()
{
	gl_PointSize = 1.0;
    gl_Position = projection * view * model * vec4(inPositionAndRemainingLifetime.xyz, 1.0);
	lifetime = inPositionAndRemainingLifetime.w;
}
