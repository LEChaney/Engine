#include "ParticleSystem.h"

#include "ParticleEmitterComponent.h"
#include "GLUtils.h"

#include <glad\glad.h>

ParticleSystem::ParticleSystem(Scene& scene)
	: System(scene)
{
}


void ParticleSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);

		if (entity.hasComponents(COMPONENT_PARTICLE_EMITTER)) {
			ParticleEmitterComponent emitter = entity.particleEmitter;

			GLUtils::getPhysicsComputeShader().use();
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, emitter.particleBuffer);
			glDispatchCompute(emitter.numParticles / 128, 1, 1);
		}
	}
}

void ParticleSystem::beginFrame()
{
}

void ParticleSystem::endFrame()
{
}
