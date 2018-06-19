#pragma once

#include "Texture.h"
#include "RenderBuffer.h"
#include "FrameBuffer.h"
#include "Shader.h"

#include <GLFW\glfw3.h>
#include <glad\glad.h>

#include <vector>

class Entity;

struct RenderState {
	GLFWwindow* glContext;
	const Entity* cameraEntity;
	GLuint radianceMap;
	bool hasRadianceMap;
	GLuint irradianceMap;
	bool hasIrradianceMap;
	GLuint uboUniforms; // TODO: Remove this and replace with more specific UBOs that can be buffered at different times to remove uneccessary buffering from CPU to GPU.
	GLuint uboCameraData;
	FrameBuffer sceneFramebuffer;
	Texture sceneColorBuffer;
	RenderBuffer sceneDepthStencilBuffer;
	const Shader* postProcessShader;
	GLuint uniformBindingPoint;
	std::vector<const Entity*> spotlights;
};