#include "ClothPickingSystem.h"

#include "Scene.h"
#include "Entity.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <limits>

using namespace glm;

ClothPickingSystem::ClothPickingSystem(Scene& scene, Entity& cameraEntity)
	: System(scene)
	, m_cameraEntity{ &cameraEntity }
{
	glfwGetCursorPos(glfwGetCurrentContext(), &m_mousePos.x, &m_mousePos.y);
	m_prevMousePos = m_mousePos;
}

void ClothPickingSystem::update()
{
	int leftMouseState = glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT);
	int rightMouseState = glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT);
	glfwGetCursorPos(glfwGetCurrentContext(), &m_mousePos.x, &m_mousePos.y);

	// Start burning point on right click
	if (m_prevRightMouseState == GLFW_RELEASE && rightMouseState == GLFW_PRESS) {
		PointMass* pointMass = mousePick();
		if (pointMass) {
			pointMass->isBurning = true;
		}
	}

	// Grab point mass on mouse button down
	if (m_prevLeftMouseState == GLFW_RELEASE && leftMouseState == GLFW_PRESS) {
		m_grabbedPointMass = mousePick();
		if (m_grabbedPointMass) {
			if (m_grabbedPointMass->isFixed)
				m_grabbedPointMass = nullptr;
			else
				m_grabbedPointMass->isFixed = true;
		}
	}

	// Release point mass on mouse button up
	if (m_grabbedPointMass && m_prevLeftMouseState == GLFW_PRESS && leftMouseState == GLFW_RELEASE) {
		m_grabbedPointMass->isFixed = false;
		m_grabbedPointMass = nullptr;
	}

	// Drag grabbed point mass around when mouse is held down
	if (leftMouseState == GLFW_PRESS && m_grabbedPointMass) {
		updateGrabbedPointMass();
	}

	m_prevLeftMouseState = leftMouseState;
	m_prevRightMouseState = rightMouseState;
	m_prevMousePos = m_mousePos;
}

void ClothPickingSystem::beginFrame()
{
}

void ClothPickingSystem::endFrame()
{
}

PointMass* ClothPickingSystem::mousePick()
{
	/**********************************/
	/** Construct ray in world space **/
	/**********************************/

	glm::dvec2 mousePos;
	glfwGetCursorPos(glfwGetCurrentContext(), &mousePos.x, &mousePos.y);

	int width;
	int height;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Get mouse position in NDC space
	glm::vec2 mousePosNDC;
	mousePosNDC.x = 2 * static_cast<float>(mousePos.x) / width - 1;
	mousePosNDC.y = 1 - 2 * static_cast<float>(mousePos.y) / height;

	// Clip space
	glm::vec4 clipCoords = glm::vec4(mousePosNDC.x, mousePosNDC.y, 1, 1);

	// View space
	mat4 inverseProj = glm::inverse(m_cameraEntity->camera.getProjection());
	glm::vec4 eyeCoords = inverseProj * clipCoords;
	//eyeCoords /= eyeCoords.w;
	//eyeCoords.z = -1;
	eyeCoords.w = 0;

	// World space
	const mat4& inverseView = glm::inverse(m_cameraEntity->camera.getView());
	glm::vec4 rayDir4 = inverseView * eyeCoords;
	glm::vec3 rayDir = rayDir4;
	rayDir = glm::normalize(rayDir);
	glm::vec3 rayOrigin = m_cameraEntity->camera.getPosition();


	/****************************************/
	/** Perform raytrace of cloth entities **/
	/****************************************/

	float closestIntersectionDist = std::numeric_limits<float>::max();
	PointMass* closestPointMass = nullptr;
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& clothEntity = m_scene.getEntity(i);

		if (!clothEntity.hasComponents(COMPONENT_CLOTH))
			continue;

		ClothComponent& cloth = clothEntity.cloth;
		auto& triIndices = cloth.triIndices;
		
		for (size_t i = 0; i < cloth.triIndices.size(); i += 3) {
			const float EPSILON = 0.0000001;
			PointMass& p0 = cloth.getNode(triIndices[i]).pointMass;
			PointMass& p1 = cloth.getNode(triIndices[i + 1]).pointMass;
			PointMass& p2 = cloth.getNode(triIndices[i + 2]).pointMass;
			vec3 edge1, edge2, h, s, q;
			float a, f, u, v;
			edge1 = p1.getPosition() - p0.getPosition();
			edge2 = p2.getPosition() - p0.getPosition();
			h = glm::cross(rayDir, edge2);
			a = glm::dot(edge1, h);
			if (a > -EPSILON && a < EPSILON)
				continue;
			f = 1 / a;
			s = rayOrigin - p0.getPosition();
			u = f * glm::dot(s, h);
			if (u < 0.0 || u > 1.0)
				continue;
			q = glm::cross(s, edge1);
			v = f * glm::dot(rayDir, q);
			if (v < 0.0 || u + v > 1.0)
				continue;
			// At this stage we can compute t to find out where the intersection point is on the line.
			float t = f * glm::dot(edge2, q);
			if (t > EPSILON) // ray intersection
			{
				// Check if this intersection point is nearer than any found so far
				if (t < closestIntersectionDist) {
					closestIntersectionDist = t;
					vec3 intersectionPoint = rayOrigin + rayDir * closestIntersectionDist;

					// Update closest point mass to intersection point
					closestPointMass = &p0;
					for (size_t j = 1; j < 3; ++j) {
						if (glm::length(p1.getPosition() - intersectionPoint) < glm::length(closestPointMass->getPosition() - intersectionPoint))
							closestPointMass = &p1;
						if (glm::length(p2.getPosition() - intersectionPoint) < glm::length(closestPointMass->getPosition() - intersectionPoint))
							closestPointMass = &p2;
					}
				}
			}
		}
	}

	return closestPointMass;
}

void ClothPickingSystem::updateGrabbedPointMass()
{
	if (!m_grabbedPointMass)
		return;

	int width;
	int height;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Grabbed point position in ndc space
	glm::vec4 grabbedPointVewPos = m_cameraEntity->camera.getView() * glm::vec4{ m_grabbedPointMass->getPosition(), 1.0f };
	glm::vec4 grabbedPointClipPos = m_cameraEntity->camera.getProjection() * grabbedPointVewPos;
	grabbedPointClipPos /= grabbedPointClipPos.w;

	// Get mouse position in NDC space
	glm::vec2 mousePosNDC;
	mousePosNDC.x = 2 * static_cast<float>(m_mousePos.x) / width - 1;
	mousePosNDC.y = 1 - 2 * static_cast<float>(m_mousePos.y) / height;

	// NDC space mouse position using the grabbed points NDC space depth
	glm::vec4 clipCoords = glm::vec4(mousePosNDC.x, mousePosNDC.y, grabbedPointClipPos.z, 1);

	// New View space position
	mat4 inverseProj = glm::inverse(m_cameraEntity->camera.getProjection());
	glm::vec4 eyeCoords = inverseProj * clipCoords;
	eyeCoords /= eyeCoords.w;

	// New world space position
	const mat4& inverseView = glm::inverse(m_cameraEntity->camera.getView());
	glm::vec4 newPos4 = inverseView * eyeCoords;
	glm::vec3 newPos = newPos4;

	m_grabbedPointMass->setPosition(newPos);
}
