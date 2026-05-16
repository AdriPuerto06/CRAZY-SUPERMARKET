#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>
#include <vector>
//#include <memory>

class Event;
class BaseNPC;

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	std::shared_ptr<Entity> CreateEntity(EntityType type);

	void DestroyEntity(std::shared_ptr<Entity> entity);

	void AddEntity(std::shared_ptr<Entity> entity);

	std::vector<std::shared_ptr<Event>> GetEventsEntities();


	std::shared_ptr<Entity> GetEntity(EntityType type, int ID);

	std::shared_ptr<Entity> GetEnemy(int id);

	std::shared_ptr<BaseNPC> GetNPC(int id);

	std::shared_ptr<Entity> GetEntity_Map(int id, EntityType type);

	std::shared_ptr<Entity> GetCompanion(int id);

public:

	std::list<std::shared_ptr<Entity>> entities;

};
