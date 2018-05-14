#include "PhysicsWorld.h"

#include "GLUtils.h"
#include "PointMassFormat.h"
#include "Shader.h"

PhysicsWorld::PhysicsWorld()
	: m_pointMassCount{ 0 }
	, m_kMaxPointMasses{ 2000000 }
{
	GLuint bufferSize = m_kMaxPointMasses * sizeof(PointMassFormat);

	m_pointMassBuffer = GLUtils::createBuffer(bufferSize, nullptr, GL_DYNAMIC_DRAW, GL_SHADER_STORAGE_BUFFER);
}

GLuint PhysicsWorld::addPointMass(glm::vec3 position, GLfloat mass)
{
	PointMassFormat pointMass{};
	pointMass.position = glm::vec4(position, 1);
	//pointMass.mass = mass;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pointMassBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(pointMass) * m_pointMassCount, sizeof(pointMass), &pointMass);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

	++m_pointMassCount;
	return m_pointMassCount - 1;
}

void PhysicsWorld::step()
{
	const Shader& shader = GLUtils::getPhysicsComputeShader();
	shader.use();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pointMassBuffer);
	glDispatchCompute(m_pointMassCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}

GLuint PhysicsWorld::getPointMassCount()
{
	return m_pointMassCount;
}

GLuint PhysicsWorld::getPointMassBuffer()
{
	return m_pointMassBuffer;
}
