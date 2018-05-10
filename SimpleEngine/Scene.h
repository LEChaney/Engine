//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A container for all the entities / components in 
//                scene.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "Entity.h"

#include <vector>
#include <memory>

class EntityEventListener;

class Scene {
public:
	template<typename ...ComponentTs>
	Entity& createEntity(size_t firstComponent, ComponentTs... rest);
	Entity& createEntity(size_t componentType);
	void destroyEntity(Entity&);
	Entity& getEntity(size_t entityID);
	size_t getEntityCount();
	static void makeSceneCurrent(Scene* scene);
	static Scene* getCurrentScene();
	void registerEntityEventListener(EntityEventListener*);
	void removeEntityEventListener(EntityEventListener*);

private:
	// TODO: Change this to be a vector of 'observable' entities
	// that can be observed robustly, even when a vector resize causes the
	// entities to shift in memory.
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::vector<EntityEventListener*> m_eventListeners;
	static Scene* s_currentScene;

	void triggerEntityCreationEvent(Entity&);
	void triggerEntityDestructionEvent(Entity&);
};

template<typename ...ComponentTs>
inline Entity& Scene::createEntity(size_t firstComponent, ComponentTs... rest)
{
	size_t componentMask = Entity::assembleComponentMask(firstComponent, rest...);
	Entity& entity = createEntity(componentMask);
	return entity;
}
