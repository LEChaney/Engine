#pragma once

#include "System.h"

#include <glad\glad.h>

class Scene;

class CollisionSystem : public System {
public:
	CollisionSystem(Scene&);
	~CollisionSystem() override;
	CollisionSystem(const CollisionSystem&) = delete;
	CollisionSystem& operator=(const CollisionSystem&) = delete;

	// Inherited via System
	virtual void update() override;
	virtual void beginFrame() override;
	virtual void endFrame() override;

private:
	void SphereCollision(Entity& clothEntity, Entity& sphereEntity);
	void PyramidCollision(Entity& clothEntity, Entity& pyramidEntity);
	std::vector<glm::vec3> PenetratingPoints(std::vector<glm::vec3> penetratingTriangle, std::vector<glm::vec3> triangle);
	void PointLineDistance(glm::vec3 point, glm::vec3 lineSeg1, glm::vec3 lineSeg2, float& distance, glm::vec3& pointOnLine);
	int TriangleTriangleIntersection(std::vector<glm::vec3>& T1, std::vector<glm::vec3>& T2, int& coplanar, 
		glm::vec3& isectpt1, glm::vec3& isectpt2);
};
