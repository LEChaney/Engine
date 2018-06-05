#include "MousePickingSystem.h"

#include "Scene.h"
#include "Entity.h"

#include <GLFW\glfw3.h>

MousePickingSystem::MousePickingSystem(Scene& scene)
	: System(scene)
{
}

void MousePickingSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& clothEntity = m_scene.getEntity(i);

		if (!clothEntity.hasComponents(COMPONENT_CLOTH))
			continue;

		// Mouse picking on left mouse button down
		int mouseState = glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT);
		if (mouseState == GLFW_PRESS) {

		}
	}
}

void MousePickingSystem::beginFrame()
{
}

void MousePickingSystem::endFrame()
{
}
