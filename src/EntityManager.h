#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>

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

	std::shared_ptr<Entity> GetEntity(EntityType type, int ID);
	std::shared_ptr<Entity> GetNPC(int ID);

public:

	std::list<std::shared_ptr<Entity>> entities;

};
