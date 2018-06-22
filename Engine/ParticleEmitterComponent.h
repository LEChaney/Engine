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

#pragma once

#include "Material.h"

#include <glm\glm.hpp>
#include <glad\glad.h>

class Entity;
class Scene;

struct ParticleEmitterComponent {
	GLuint numParticles;
	Material material;
	GLuint particleBuffer;
	GLuint VAO;
};

namespace Prefabs {
	Entity& createParticleEmitter(Scene&, GLuint numParticles, const GLfloat minLifetime, const GLfloat maxLifetime, const glm::vec3& minInitPos, const glm::vec3& maxInitPos, const glm::vec3& minInitVel, const glm::vec3& maxInitVel);
}