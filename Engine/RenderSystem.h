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

#pragma once

#include "RenderState.h"
#include "EntityEventListener.h"
#include "System.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>

class Scene;
struct GLFWwindow;
class Entity;
struct ModelComponent;
struct ParticleEmitterComponent;
class Shader;

class RenderSystem : public System, public EntityEventListener {
public:
	RenderSystem(Scene&);
	~RenderSystem() override;
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	// Draws a debugging arrow
	// This object will only be drawn once.
	// To keep or update the arrow, drawDebugArrow must be called every frame.
	static void drawDebugArrow(const glm::vec3& base, const glm::vec3& tip,
		const glm::vec3& color = glm::vec3(1, 0, 0));

	// Draws a debugging arrow
	// This object will only be drawn once.
	// To keep or update the arrow, drawDebugArrow must be called every frame.
	static void drawDebugArrow(const glm::vec3& base, const glm::vec3& direction, 
		float magnitude, const glm::vec3& color = glm::vec3(1, 0, 0));

	// Starts rendering the frame.
	// Should be called before update.
	void beginFrame() override;

	// Renders an entity.
	void update() override;

	// Ends the frame.
	void endFrame() override;

	// Sets the current camera.
	// Also sets the static debug camera for debug drawing.
	void setCamera(const Entity*);

	// Sets the radiance map for reflections
	void setRadianceMap(GLuint radianceMap);

	// Sets the irradiance map for image based lighting
	void setIrradianceMap(GLuint irradianceMap);

	// Detect lights added to the scene
	virtual void onPostAddComponents(Entity& entity, size_t componentMaskAdded) override;

	// Detect lights removed from the scene
	virtual void onPreRemoveComponents(Entity& entity, size_t componentMaskToRemove) override;

private:
	static void renderModel(const ModelComponent&, const glm::mat4& transform, const glm::mat4* view = nullptr, const glm::mat4* projection = nullptr, bool isShadowPass = false);

	static void renderParticles(const ParticleEmitterComponent&, const glm::mat4 transform);

	// Buffers light data and sets lightSpaceMatrix for shadow mapping on GPU
	static void bufferLightData(const glm::mat4& lightSpaceMatrix);

	static RenderState* s_renderState;
	RenderState m_renderState;
	std::vector<const Shader*> m_postProcessShaders;
	GLsizei m_curPostProcessShaderIdx;
};
