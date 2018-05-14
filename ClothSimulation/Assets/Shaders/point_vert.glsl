#version 420 core

layout (location = 0) in vec4 inWorldPosition;

layout (std140, binding=0) uniform CameraData {
    mat4 view;
    mat4 projection;
} cam;

void main()
{
	gl_PointSize = 1.0;
    gl_Position = cam.projection * cam.view * inWorldPosition;
}
