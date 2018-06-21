#include "ParticleEmitterComponent.h"

#include "GLUtils.h"
#include "ParticleFormat.h"
#include "Scene.h"

#include <glm\gtc\random.hpp>

Entity& Prefabs::createParticleEmitter(Scene& scene, GLuint numParticles, const GLfloat minLifetime, const GLfloat maxLifetime, const glm::vec3& minInitPos, const glm::vec3& maxInitPos, const glm::vec3& minInitVel, const glm::vec3 & maxInitVel)
{
	ParticleEmitterComponent emitter;
	emitter.numParticles = numParticles;
	emitter.material.shader = &GLUtils::getPointShader();

	// Create particle buffer
	emitter.particleBuffer = GLUtils::createBuffer(sizeof(ParticleFormat) * numParticles);

	// Map GPU particle buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, emitter.particleBuffer);
	auto particles = static_cast<ParticleFormat*>(glMapBufferRange(
		GL_SHADER_STORAGE_BUFFER,
		0,
		sizeof(ParticleFormat) * numParticles,
		GL_MAP_WRITE_BIT
	));

	// Assign values to each particle on the GPU
	for (GLuint i = 0; i < numParticles; ++i) {
		GLfloat lifetime = glm::linearRand(minLifetime, maxLifetime);
		particles[i].positionAndRemainingLifetime = glm::vec4(glm::linearRand(minInitPos, maxInitPos), lifetime);
		particles[i].velocityAndInitLifetime = glm::vec4(glm::linearRand(minInitVel, maxInitVel), lifetime);
	}

	// Unmap GPU particle buffer
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Setup VAO / VBO
	glGenVertexArrays(1, &emitter.VAO);
	glBindVertexArray(emitter.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, emitter.particleBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleFormat), 0);
	glEnableVertexAttribArray(0);

	Entity& emitterEntity = scene.createEntity(COMPONENT_TRANSFORM, COMPONENT_PARTICLE_EMITTER);
	emitterEntity.particleEmitter = emitter;
	return emitterEntity;
}