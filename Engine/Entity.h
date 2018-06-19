#pragma once

#include "InputComponent.h"
#include "ModelComponent.h"
#include "VehicleMovementComponent.h"
#include "InputMapComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "BasicCameraMovementComponent.h"
#include "Terrain.h"
#include "TerrainFollowComponent.h"
#include "SimpleWorldSpaceMoveComponent.h"
#include "ClothComponent.h"
#include "SphereCollisionComponent.h"
#include "PyramidCollisionComponent.h"
#include "CapsuleCollisionComponent.h"
#include "DirectionalLightComponent.h"

#include <vector>

class EntityEventListener;
 
enum ComponentType {
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_MODEL = 1 << 2,
	COMPONENT_CAMERA = 1 << 3,
	COMPONENT_INPUT = 1 << 5,
	COMPONENT_INPUT_MAP = 1 << 6,
	COMPONENT_BASIC_CAMERA_MOVEMENT = 1 << 10,
	COMPONENT_TERRAIN = 1 << 11,
	COMPONENT_TERRAIN_FOLLOW = 1 << 12,
	COMPONENT_SIMPLE_WORLD_SPACE_MOVE_COMPONENT = 1 << 13,
	COMPONENT_CLOTH = 1 << 14,
	COMPONENT_SPHERE_COLLISION = 1 << 15,
	COMPONENT_PYRAMID_COLLISION = 1 << 16,
	COMPONENT_GROUND_COLLISION = 1 << 17,
	COMPONENT_CAPSULE_COLLISION = 1 << 18,
	COMPONENT_DIRECTIONAL_LIGHT = 1 << 19
};

class Entity {
	// The scene will handle all entity creation and destruction
	friend class Scene;

public:
	TransformComponent transform;
	PhysicsComponent physics;
	ModelComponent model;
	VehicleMovementComponent vehicleMovement;
	InputComponent input;
	InputMapComponent inputMap;
	CameraComponent camera;
	BasicCameraMovementComponent basicCameraMovement;
	TerrainComponent terrain;
	TerrainFollowComponent terrainFollow;
	SimpleWorldSpaceMoveComponent simpleWorldSpaceMovement;
	ClothComponent cloth;
	SphereCollisionComponent sphereCollision;
	PyramidCollisionComponent pyramidCollision;
	CapsuleCollisionComponent capsuleCollision;
	DirectionalLightComponent directionalLight;

	Entity(Entity&&) = default;
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	// Overload equality to check identity equivalence
	bool operator==(const Entity& rhs) const;

	// Returns true if ALL the specified components are present 
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponents(size_t first, ComponentTs... rest) const;

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	bool hasComponents(size_t componentMask) const;

	// Returns true if ANY of the specified components are present
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponentsAny(size_t first, ComponentTs... rest) const;

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	bool hasComponentsAny(size_t componentMask) const;

	// Returns true if this entity has any components
	bool hasComponents() const;

	// Adds multiple components to the entity
	template<typename ...ComponentTs>
	void addComponents(size_t first, ComponentTs... rest);

	// Adds a component to the entity.
	// A mask can also be specified to add more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void addComponents(size_t componentMask);

	// Removes a component from the entity.
	// A mask can also be specified to remove more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void removeComponents(size_t componentMask);

	// Removes multiple components from the entity
	template<typename ...ComponentTs>
	void removeComponents(size_t first, ComponentTs... rest);

	// Assembles the component mask from multiple arguments
	template<typename ...ComponentTs>
	static size_t assembleComponentMask(size_t first, ComponentTs... rest);
	static size_t assembleComponentMask(size_t componentMask);

	// Returns true if ALL the specified components are present 
	// in the componentMask.
	template<typename ...ComponentTs>
	static bool componentMaskContains(size_t componentMask, size_t first, ComponentTs... rest);

	// Returns true if ALL the specified components are present 
	// in the componentMask.
	static bool componentMaskContains(size_t componentMask, size_t containedComponentMask);

	// Returns true if ANY of the specified components are present
	// in the componentMask.
	template<typename ...ComponentTs>
	static bool componentMaskContainsAny(size_t componentMask, size_t first, ComponentTs... rest);

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	static bool componentMaskContainsAny(size_t componentMask, size_t containedComponentMask);

	void triggerPostAddComponentsEvent(size_t componentMask);
	void triggerPreRemoveComponentsEvent(size_t componentMask);

private:
	Entity(std::vector<EntityEventListener*>& eventListeners);

	// Destroys to entity
	void destroy();

	size_t m_componentMask;
	std::vector<EntityEventListener*>& m_eventListeners;
};

template<typename ...ComponentTs>
inline bool Entity::hasComponents(size_t first, ComponentTs... rest) const
{
	return hasComponents(first) && hasComponents(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::hasComponentsAny(size_t first, ComponentTs ...rest) const
{
	return hasComponentsAny(first) || hasComponentsAny(rest...);
}

template<typename ...ComponentTs>
inline void Entity::addComponents(size_t first, ComponentTs... rest)
{
	size_t componentMask = assembleComponentMask(first, rest...);
	addComponents(componentMask);
}

template<typename ...ComponentTs>
inline void Entity::removeComponents(size_t first, ComponentTs ...rest)
{
	size_t componentMask = assembleComponentMask(first, rest...);
	removeComponents(componentMask);
}

template<typename ...ComponentTs>
inline size_t Entity::assembleComponentMask(size_t first, ComponentTs ...rest)
{
	return first | assembleComponentMask(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::componentMaskContains(size_t componentMask, size_t first, ComponentTs ...rest)
{
	return componentMaskContains(componentMask, first) && componentMaskContains(componentMask, rest...);
}

template<typename ...ComponentTs>
inline bool Entity::componentMaskContainsAny(size_t componentMask, size_t first, ComponentTs ...rest)
{
	return componentMaskContainsAny(componentMask, first) || componentMaskContainsAny(componentMask, rest...);
}
