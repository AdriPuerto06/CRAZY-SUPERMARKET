#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"

#include "BaseEnemy.h"
#include "BaseNPC.h"

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Destroy();
	}

	return ret;
}

std::shared_ptr<Entity> EntityManager::CreateEntity(EntityType type)
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();

	//L04: TODO 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	//Añadir las clases concretas de los enemigos y npcs cuando las tengamos
	switch (type)
	{
	case EntityType::PLAYER:
		entity = std::make_shared<Player>();
		break;
	case EntityType::ITEM:
		entity = std::make_shared<Item>();
		break;
	case EntityType::BASEENEMY:
		entity = std::make_shared<BaseEnemy>();
		break;
	case EntityType::BASENPC:
		entity = std::make_shared<BaseNPC>();
		break;
	default:
		break;
	}

	entities.push_back(entity);

	return entity;
}

void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity)
{
	entity->CleanUp();
	entities.remove(entity);
}

void EntityManager::AddEntity(std::shared_ptr<Entity> entity)
{
	if ( entity != nullptr) entities.push_back(entity);
}

std::shared_ptr<Entity> EntityManager::GetEntity(EntityType type, int ID)
{
	for (std::shared_ptr<Entity> entity : entities)
	{
		if (entity->entity_ID == ID && entity->type == type)
		{
			return entity;
		}
	}
	return nullptr;
}

std::shared_ptr<Entity> EntityManager::GetEnemy(int id) {
	for (auto& entity : entities) {
		auto enemy = std::dynamic_pointer_cast<BaseEnemy>(entity);
		if (enemy && enemy->ID == id)
			return enemy;
	}
	return nullptr;
}

std::shared_ptr<Entity> EntityManager::GetNPC(int id) {
	for (auto& entity : entities) {
		auto enemy = std::dynamic_pointer_cast<BaseNPC>(entity);
		if (enemy && enemy->ID == id)
			return enemy;
	}
	return nullptr;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;

	//List to store entities pending deletion
	std::list<std::shared_ptr<Entity>> pendingDelete;
	
	//Iterates over the entities and calls Update
	for(const auto entity : entities)
	{
		//If the entity is marked for deletion, add it to the pendingDelete list
		if (entity->pendingToDelete)
		{
			pendingDelete.push_back(entity);
		}
		//If the entity is not active, skip it
		if (entity->active == false) continue;
		ret = entity->Update(dt);
	}

	//Now iterates over the pendingDelete list and destroys the entities
	for (const auto entity : pendingDelete)
	{
		DestroyEntity(entity);
	}

	return ret;
}