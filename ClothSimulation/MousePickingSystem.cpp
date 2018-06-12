#include "MousePickingSystem.h"

#include "Scene.h"
#include "Entity.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <limits>

using namespace glm;

MousePickingSystem::MousePickingSystem(Scene& scene, Entity& cameraEntity)
	: System(scene)
	, m_cameraEntity{ &cameraEntity }
{
	glfwGetCursorPos(glfwGetCurrentContext(), &m_mousePos.x, &m_mousePos.y);
	m_prevMousePos = m_mousePos;
}

void MousePickingSystem::update()
{
	int mouseState = glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT);
	glfwGetCursorPos(glfwGetCurrentContext(), &m_mousePos.x, &m_mousePos.y);

	// Grab point mass on mouse button down
	if (m_prevMouseState == GLFW_RELEASE && mouseState == GLFW_PRESS) {
		m_grabbedPointMass = mousePick();
		if (m_grabbedPointMass) {
			if (m_grabbedPointMass->isFixed)
				m_grabbedPointMass = nullptr;
			else
				m_grabbedPointMass->isFixed = true;
		}
	}

	// Release point mass on mouse button up
	if (m_grabbedPointMass && m_prevMouseState == GLFW_PRESS && mouseState == GLFW_RELEASE) {
		m_grabbedPointMass->isFixed = false;
		m_grabbedPointMass = nullptr;
	}

	// Drag grabbed point mass around when mouse is held down
	if (mouseState == GLFW_PRESS && m_grabbedPointMass) {
		updateGrabbedPointMass();
	}

	m_prevMouseState = mouseState;
	m_prevMousePos = m_mousePos;
}

void MousePickingSystem::beginFrame()
{
}

void MousePickingSystem::endFrame()
{
}

PointMass* MousePickingSystem::mousePick()
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

void MousePickingSystem::updateGrabbedPointMass()
{
	if (!m_grabbedPointMass)
		return;

	glm::vec4 viewPos = m_cameraEntity->camera.getView() * glm::vec4{ m_grabbedPointMass->getPosition(), 1.0f };
	glm::vec2 deltaMouse = m_mousePos - m_prevMousePos;

	viewPos.x += deltaMouse.x * 0.05f;
	viewPos.y -= deltaMouse.y * 0.05f;

	glm::vec4 newPos = glm::inverse(m_cameraEntity->camera.getView()) * viewPos;
	m_grabbedPointMass->setPosition(newPos);
}
