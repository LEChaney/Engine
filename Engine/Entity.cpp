#include "Entity.h"

#include "EntityEventListener.h"

#include <glm\glm.hpp>

#include "Log.h"

Entity::Entity(std::vector<EntityEventListener*>& eventListeners)
	: m_componentMask{ 0 }
	, m_eventListeners{ eventListeners }
	, transform{}
	, physics{}
	, model{}
	, vehicleMovement{}
	, input{}
	, inputMap{}
	, camera{}
	, basicCameraMovement{}
	, terrain{}
	, terrainFollow{}
	, simpleWorldSpaceMovement{}
	, cloth{}
	, sphereCollision{}
	, directionalLight{}
{
}

void Entity::destroy()
{
	triggerPreRemoveComponentsEvent(m_componentMask);
	m_componentMask = 0;

	m_componentMask = { 0 };
	transform = {};
	physics = {};
	model = {};
	vehicleMovement = {};
	input = {};
	inputMap = {};
	camera = {};
	basicCameraMovement = {};
	terrain = {};
	terrainFollow = {};
	simpleWorldSpaceMovement = {};
	cloth = {};
	sphereCollision = {};
	directionalLight = {};
}

bool Entity::operator==(const Entity& rhs) const
{
	return this == &rhs;
}

bool Entity::hasComponents(size_t componentMask) const
{
	return (m_componentMask & componentMask) == componentMask;
}

bool Entity::hasComponentsAny(size_t componentMask) const
{
	return (m_componentMask & componentMask) > 0;
}

bool Entity::hasComponents() const
{
	return m_componentMask != 0;
}

bool Entity::componentMaskContains(size_t componentMask, size_t containedComponentMask)
{
	return (componentMask & containedComponentMask) == containedComponentMask;
}

bool Entity::componentMaskContainsAny(size_t componentMask, size_t containedComponentMask)
{
	return (componentMask & containedComponentMask) > 0;
}

void Entity::triggerPostAddComponentsEvent(size_t componentMask)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onPostAddComponents(*this, componentMask);
	}
}

void Entity::triggerPreRemoveComponentsEvent(size_t componentMask)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onPreRemoveComponents(*this, componentMask);
	}
}

void Entity::addComponents(size_t componentMask)
{
	m_componentMask |= componentMask;

	triggerPostAddComponentsEvent(componentMask);
}

void Entity::removeComponents(size_t componentMask)
{
	triggerPreRemoveComponentsEvent(componentMask);
	m_componentMask &= (~componentMask);
}

size_t Entity::assembleComponentMask(size_t componentMask)
{
	return componentMask;
}
