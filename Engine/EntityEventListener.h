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

class Entity;

class EntityEventListener {
public:
	// Triggers just after the components in the component mask have been added
	virtual void onPostAddComponents(Entity& entity, size_t componentMaskAdded) { };
	// Triggers just before the components in the component mask are removed
	virtual void onPreRemoveComponents(Entity& entity, size_t componentMaskToRemove) { };
	// Triggered when an entity is created in the scene.
	// This event will fire after the entity has been fully initialized.
	virtual void onEntityCreation(Entity& entity) { };
	// Triggered when an entity is destroyed in the scene.
	// The event will fire just before the entity is actually destroyed,
	// so the state of the entity will still be valid when this event
	// fires.
	virtual void onEntityDestruction(Entity& entity) { };
};