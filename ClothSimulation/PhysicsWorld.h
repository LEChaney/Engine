#pragma once

#include <glm\glm.hpp>
#include <glad\glad.h>

#include <array>
#include <vector>

class PhysicsWorld {
public:
	PhysicsWorld();

	// void applyImpulse(GLuint hBody, const glm::vec3& impulse);
	// void applyForce(GLuint hBody, const glm::vec3& force);
	GLuint addPointMass(glm::vec3 position, GLfloat mass);
	void step();
	//GLuint addSpringConstraint(GLuint bodyID1, GLuint bodyID2, GLfloat stiffness, GLfloat damping);

	GLuint getPointMassCount();
	GLuint getPointMassBuffer();

private:
	//std::vector<GLuint> m_physBodyIDs;
	//std::vector<GLuint> m_springIDs;
	GLuint m_pointMassBuffer;
	GLuint m_pointMassCount;
	const GLuint m_kMaxPointMasses;
};