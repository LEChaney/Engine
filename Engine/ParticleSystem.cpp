//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

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
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, emitter.particleBuffer);
			glDispatchCompute(emitter.numParticles / 128, 1, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}
	}
}

void ParticleSystem::beginFrame()
{
}

void ParticleSystem::endFrame()
{
}
