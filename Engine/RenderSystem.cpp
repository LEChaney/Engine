//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles rendering each entity.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "RenderSystem.h"

#include "Utils.h"
#include "GLUtils.h"
#include "GLMUtils.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Entity.h"
#include "UniformBlockFormat.h"
#include "UBOBlockFormats.h"
#include "PrimitivePrefabs.h"
#include "ModelUtils.h"
#include "Game.h"
#include "RenderBuffer.h"
#include "FrameBuffer.h"
#include "GLPrimitives.h"
#include "Clock.h"
#include "Shader.h"
#include "LightDataBlockFormat.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\euler_angles.hpp>

#include <algorithm>
#include <cmath>

using glm::mat4;
using glm::vec3;
using glm::vec4;

RenderState* RenderSystem::s_renderState = nullptr;

RenderSystem::RenderSystem(Scene& scene)
	: System{ scene }
{
	m_renderState.glContext = Game::getWindowContext();
	m_renderState.uniformBindingIndex = 0;
	m_renderState.lightDataBindingIndex = 1;
	m_renderState.hasIrradianceMap = false;
	m_renderState.hasRadianceMap = false;

	// Set post processing shader
	m_postProcessShaders.push_back(&GLUtils::getFullscreenQuadShader());
	m_postProcessShaders.push_back(&GLUtils::getPPEdgeDetectShader());
	m_curPostProcessShaderIdx = 0;
	m_renderState.postProcessShader = m_postProcessShaders[m_curPostProcessShaderIdx];

	// Scene framebuffer
	FrameBuffer& framebuffer = m_renderState.sceneFramebuffer;
	glGenFramebuffers(1, &framebuffer.id);
	glBindFramebuffer(framebuffer.target, framebuffer.id);

	// Scene Color buffer
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(m_renderState.glContext, &windowWidth, &windowHeight);
	Texture& colorBuffer = m_renderState.sceneColorBuffer = Texture::Texture2D(windowWidth, windowHeight);
	glFramebufferTexture2D(framebuffer.target, GL_COLOR_ATTACHMENT0, colorBuffer.target, colorBuffer.id, 0);

	// Scene Depth and Stencil buffers
	m_renderState.sceneDepthStencilBuffer = RenderBuffer(GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	RenderBuffer& depthStencilBuffer = m_renderState.sceneDepthStencilBuffer;
	glFramebufferRenderbuffer(framebuffer.target, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilBuffer.target, depthStencilBuffer.id);

	assert(glCheckFramebufferStatus(framebuffer.target) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(framebuffer.target, 0);

	// Shadow framebuffer
	FrameBuffer& shadowMapFramebuffer = m_renderState.shadowMapFramebuffer;
	glGenFramebuffers(1, &shadowMapFramebuffer.id);
	glBindFramebuffer(shadowMapFramebuffer.target, shadowMapFramebuffer.id);

	// Shadow map buffer
	Texture& shadowMapBuffer = m_renderState.shadowMapBuffer = Texture::Texture2D(8192, 8192, GL_DEPTH_COMPONENT, GL_FLOAT);
	glBindTexture(shadowMapBuffer.target, shadowMapBuffer.id);
	glTexParameteri(shadowMapBuffer.target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(shadowMapBuffer.target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(shadowMapBuffer.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(shadowMapBuffer.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(shadowMapFramebuffer.target, GL_DEPTH_ATTACHMENT, shadowMapBuffer.target, shadowMapBuffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	assert(glCheckFramebufferStatus(shadowMapFramebuffer.target) == GL_FRAMEBUFFER_COMPLETE);
	glBindTexture(shadowMapBuffer.target, 0);
	glBindFramebuffer(shadowMapFramebuffer.target, 0);

	// Create buffer for uniformBlock
	glGenBuffers(1, &m_renderState.uboUniforms);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_renderState.uboUniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlockFormat), nullptr, GL_DYNAMIC_DRAW);

	// Create buffer for cameraDataBlock
	glGenBuffers(1, &m_renderState.uboCameraData);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_renderState.uboCameraData);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraDataBlockFormat), nullptr, GL_DYNAMIC_DRAW);

	// Create buffer for light data
	glGenBuffers(1, &m_renderState.uboLightData);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_renderState.uboLightData);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightDataBlockFormat), nullptr, GL_DYNAMIC_DRAW);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	m_scene.registerEntityEventListener(this);
}

RenderSystem::~RenderSystem()
{
	glDeleteFramebuffers(1, &m_renderState.sceneFramebuffer.id);
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& tip,
	const glm::vec3& color)
{
	vec3 direction = tip - base;
	drawDebugArrow(base, direction, glm::length(direction));
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& _direction, 
	float magnitude, const glm::vec3& color)
{
	mat4 transform;
	vec3 direction = glm::normalize(_direction);
	transform[1] = vec4(direction, 1);
	// Find a perpendicular vector
	glm::vec3 tangent = vec3{ 1, 1, 1 };
	if (direction.x != 0)
		tangent.x = -(direction.y + direction.z) / direction.x;
	else if (direction.y != 0)
		tangent.y = -(direction.x + direction.z) / direction.y;
	else
		tangent.z = -(direction.y + direction.z) / direction.z;
	transform[0] = vec4(glm::normalize(glm::cross(direction, tangent)), 1);
	transform[2] = vec4(glm::normalize(glm::cross(vec3(transform[0]), direction)), 1);
	transform[3] = vec4(base, 1);

	transform *= glm::scale({}, vec3(1, magnitude, 1));

	ModelComponent& model = ModelUtils::loadModel("Assets/Models/red_arrow/red_arrow.obj");
	for (size_t i = 0; i < model.materials.size(); ++i) {
		model.materials.at(i).shader = &GLUtils::getDebugShader();
		model.materials.at(i).debugColor = color;
	}

	// Can't render anything without a camera set
	if (!s_renderState->cameraEntity) {
		return;
	}

	renderModel(model, transform);
}

void RenderSystem::beginFrame()
{
	// Swap the current global render state with this RenderSystems state.
	s_renderState = &m_renderState;
}

void RenderSystem::endFrame()
{
	// Swap the post process shader on keypress
	static bool lastState = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE);
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS && lastState == GLFW_RELEASE) {
		m_curPostProcessShaderIdx = (m_curPostProcessShaderIdx + 1) % m_postProcessShaders.size();
		m_renderState.postProcessShader = m_postProcessShaders[m_curPostProcessShaderIdx];
	}
	lastState = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE);

	// Bind and clear the default framebuffer
	glBindFramebuffer(m_renderState.sceneFramebuffer.target, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render full screen quad with post process shader
	m_renderState.postProcessShader->use();
	glDisable(GL_DEPTH_TEST);
	const Mesh& quadMesh = GLPrimitives::getQuadMesh();
	glBindVertexArray(quadMesh.VAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_renderState.postProcessShader->getUniformLocation("sceneSampler"), 0);
	glBindTexture(m_renderState.sceneColorBuffer.target, m_renderState.sceneColorBuffer.id);
	glDrawElements(GL_TRIANGLES, quadMesh.numIndices, GL_UNSIGNED_INT, 0);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindTexture(m_renderState.sceneColorBuffer.target, 0);

	glfwSwapBuffers(m_renderState.glContext);
}

void RenderSystem::update()
{
	/********************/
	/** Render Shadows **/
	/********************/

	glBindFramebuffer(m_renderState.shadowMapFramebuffer.target, m_renderState.shadowMapFramebuffer.id);
	glViewport(0, 0, m_renderState.shadowMapBuffer.width, m_renderState.shadowMapBuffer.width);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	if (m_renderState.directionalLights.size() > 0) {

		// Shadow matrices
		float near_plane = 0.1f, far_plane = 2000.0f;
		glm::vec3 lightLoc = glm::normalize(m_renderState.directionalLights[0]->directionalLight.direction) * 1000.0f;
		glm::mat4 lightView = glm::lookAt(lightLoc, { 0, 0, 0 }, { 0, 1, 0 });
		glm::mat4 lightProjection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, near_plane, far_plane);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		bufferLightData(lightSpaceMatrix);

		for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
			Entity& entity = m_scene.getEntity(i);

			// Filter renderable entities
			const size_t kRenderableMask = COMPONENT_MODEL;
			if (!entity.hasComponents(kRenderableMask))
				continue;

			bool hasTransform = entity.hasComponents(COMPONENT_TRANSFORM);

			// Render current entity to shadow map
			renderModel(entity.model, GLMUtils::transformToMat(entity.transform), &lightView, &lightProjection, true);
		}
	}

	glCullFace(GL_BACK);

	/*****************/
	/** Render scene */
	/*****************/

	glBindFramebuffer(m_renderState.sceneFramebuffer.target, m_renderState.sceneFramebuffer.id);
	int width, height;
	GLFWwindow* glContext = Game::getWindowContext();
	glfwGetFramebufferSize(glContext, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);

		// Can't render anything without a camera set
		if (!m_renderState.cameraEntity) {
			return;
		}

		// Filter renderable entities
		const size_t kRenderableMask = COMPONENT_MODEL;
		if (!entity.hasComponents(kRenderableMask))
			continue;

		bool hasTransform = entity.hasComponents(COMPONENT_TRANSFORM);

		// Render the current entities model
		renderModel(entity.model, GLMUtils::transformToMat(entity.transform));
	}
}

void RenderSystem::setCamera(const Entity* entity)
{
	m_renderState.cameraEntity = entity;
}

void RenderSystem::setRadianceMap(GLuint radianceMap)
{
	m_renderState.radianceMap = radianceMap;
	m_renderState.hasRadianceMap = true;
}

void RenderSystem::setIrradianceMap(GLuint irradianceMap)
{
	m_renderState.irradianceMap = irradianceMap;
	m_renderState.hasIrradianceMap = true;
}

void RenderSystem::onPostAddComponents(Entity& entity, size_t componentMaskAdded)
{
	if (Entity::componentMaskContains(componentMaskAdded, COMPONENT_DIRECTIONAL_LIGHT)) {
		m_renderState.directionalLights.push_back(&entity);
	}
}

void RenderSystem::onPreRemoveComponents(Entity& entity, size_t componentMaskToRemove)
{
	if (Entity::componentMaskContains(componentMaskToRemove, COMPONENT_DIRECTIONAL_LIGHT)) {
		auto& lightArray = m_renderState.directionalLights;
		lightArray.erase(std::remove(lightArray.begin(), lightArray.end(), &entity));
	}
}

void RenderSystem::renderModel(const ModelComponent& model, const glm::mat4& transform, const glm::mat4* view, const glm::mat4* projection, bool isShadowPass)
{
	// Can't render without render state
	if (!s_renderState)
		return;

	// Get Aspect ratio
	int width, height;
	GLFWwindow* glContext = Game::getWindowContext();
	glfwGetFramebufferSize(glContext, &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Get model, view and projection matrices
	UniformBlockFormat uniformBlock;
	
	uniformBlock.model = transform;
	uniformBlock.view = view ? *view : s_renderState->cameraEntity->camera.getView();
	uniformBlock.projection = projection ? *projection : s_renderState->cameraEntity->camera.getProjection();
	uniformBlock.cameraPos = glm::vec4(s_renderState->cameraEntity->camera.getPosition(), 1.0f);

	// Loop over all the meshes in the model
	for (size_t i = 0; i < model.meshes.size(); ++i) {
		const Mesh& mesh = model.meshes.at(i);
		if (mesh.materialIndex >= model.materials.size()) // Check for valid material index
			continue;
		const Material& material = model.materials.at(mesh.materialIndex);

		// Enable wireframe if requested
		if (material.willDrawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Tell the gpu what shader to use
		if (isShadowPass)
			material.shader->useShadow();
		else
			material.shader->use();

		// Mostly here to ensure cubemaps don't draw on top of anything else
		if (material.willDrawDepth) {
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}
		else {
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
		}

		// Tell the gpu what diffuse textures to use
		GLuint textureUnit = 0;
		for (GLsizei j = 0; j < material.colorMaps.size(); ++j) {
			const Texture& texture = material.colorMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("texSampler" + toString(j)), textureUnit);
			glBindTexture(texture.target, texture.id);
			++textureUnit;
		}

		for (GLsizei j = 0; j < material.metallicnessMaps.size(); ++j) {
			const Texture& texture = material.metallicnessMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("metallicnessSampler"), textureUnit);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 specular texture currently
			break;
		}

		for (GLsizei j = 0; j < material.heightMaps.size(); ++j) {
			const Texture& texture = material.heightMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("heightMapSampler"), textureUnit);
			glUniform1f(material.shader->getUniformLocation("heightMapScale"), material.heightMapScale);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 height map currently
			break;
		}

		for (GLsizei j = 0; j < material.normalMaps.size(); ++j) {
			const Texture& texture = material.normalMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("normalMapSampler"), textureUnit);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 normal map currently
			break;
		}

		// Set environment map to use on GPU
		if (s_renderState->hasRadianceMap) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("radianceSampler"), textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, s_renderState->radianceMap);
			++textureUnit;
		}
		if (s_renderState->hasIrradianceMap) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("irradianceSampler"), textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, s_renderState->irradianceMap);
			++textureUnit;
		}

		// Set shader map on GPU
		if (!isShadowPass) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(material.shader->getUniformLocation("shadowMapSampler"), textureUnit);
			glBindTexture(s_renderState->shadowMapBuffer.target, s_renderState->shadowMapBuffer.id);
			++textureUnit;
		}

		// Set shader parameters
		uniformBlock.metallicness = material.shaderParams.metallicness;
		uniformBlock.glossiness = material.shaderParams.glossiness;
		uniformBlock.specBias = material.shaderParams.specBias;
		uniformBlock.time = Clock::getTime();

		// Set spotlights
		uniformBlock.numDirectionalLights = std::min(static_cast<GLuint>(s_renderState->spotlights.size()), UniformBlockFormat::s_kMaxSpotlights);
		//for (GLuint i = 0; i < uniformBlock.numSpotlights; ++i) {
		//	const Entity* spotlightEntity = s_renderState.spotlights.at(i);
		//	glm::vec4 spotlightDir = glm::vec4(s_renderState.spotlights.at(i)->spotlight.direction, 0);
		//	// Transform to local coordinates of containing entity
		//	glm::mat4 orientation = GLMUtils::eulerToMat(spotlightEntity->transform.eulerAngles);
		//	spotlightDir = orientation * spotlightDir;
		//	// Set spotlights in GPU uniform
		//	uniformBlock.spotlightDirections.at(i) = spotlightDir;
		//	uniformBlock.spotlightPositions.at(i) = glm::vec4(spotlightEntity->transform.position, 1);
		//	uniformBlock.spotlightColors.at(i) = glm::vec4(spotlightEntity->spotlight.color, 1);
		//}

		// Send uniform data to the GPU
		glUniformBlockBinding(material.shader->getGPUHandle(), material.shader->getUniformBlockIndex("UniformBlock"), s_renderState->uniformBindingIndex);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_renderState->uniformBindingIndex, s_renderState->uboUniforms);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlockFormat), &uniformBlock);
		if (material.shader == &GLUtils::getDebugShader()) {
			const glm::vec3& debugColor = material.debugColor;
			glUniform3f(material.shader->getUniformLocation("debugColor"), debugColor.r, debugColor.g, debugColor.b);
		}

		// Render the mesh
		glBindVertexArray(mesh.VAO);
		if (material.shader->hasTessellationStage()) {
			glPatchParameteri(GL_PATCH_VERTICES, 3);
			glDrawElements(GL_PATCHES, mesh.numIndices, GL_UNSIGNED_INT, 0);
		}
		else
			glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);

		// Reset GL state
		if (material.willDrawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void RenderSystem::bufferLightData(const glm::mat4& lightSpaceMatrix)
{
	LightDataBlockFormat lightData;
	lightData.lightSpaceMatrix = lightSpaceMatrix;

	lightData.numDirectionalLights = std::min(static_cast<GLuint>(s_renderState->directionalLights.size()), LightDataBlockFormat::s_kMaxDirectionalLights);
	for (GLuint i = 0; i < lightData.numDirectionalLights; ++i) {
		const Entity* directionalLightEntity = s_renderState->directionalLights.at(i);
		glm::vec4 directionalLightDir = glm::vec4(s_renderState->directionalLights.at(i)->directionalLight.direction, 0);
		glm::vec4 directionalLightColor = glm::vec4(directionalLightEntity->directionalLight.color, 1);

		// Set directional light data in GPU block format
		lightData.directionalLightDirections.at(i) = directionalLightDir;
		lightData.directionalLightColors.at(i) = directionalLightColor;
	}

	// Send light data to the GPU
	glBindBufferBase(GL_UNIFORM_BUFFER, s_renderState->lightDataBindingIndex, s_renderState->uboLightData);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightDataBlockFormat), &lightData);
}
