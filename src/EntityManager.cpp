#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
//#include "Item.h"

#include "BaseEnemy.h"
#include "BaseNPC.h"
#include "BaseCompanion.h"
#include "Event.h"

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
	//A�adir las clases concretas de los enemigos y npcs cuando las tengamos
	switch (type)
	{
	case EntityType::PLAYER:
		entity = std::make_shared<Player>();
		break;
	/*case EntityType::ITEM:
		entity = std::make_shared<Item>();
		break;*/
	case EntityType::BASEENEMY:
		entity = std::make_shared<BaseEnemy>();
		break;
	case EntityType::BASENPC:
		entity = std::make_shared<BaseNPC>();
		break;
	case EntityType::BASECOMPANION:
		entity = std::make_shared<BaseCompanion>();
		break;
	case EntityType::EVENT:
		entity = std::make_shared<Event>();
		break;
	default:
		break;
	}

	entities.push_back(entity);
	LOG("Created entity with entity_ID: %i", entity->entity_ID);

	return entity;
}

void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity)
{
	entity->CleanUp();
	entities.remove(entity);
}

void EntityManager::AddEntity(std::shared_ptr<Entity> entity)
{
	if (entity != nullptr)
	{
		entities.push_back(entity);
		LOG("Created entity with entity_ID: %i", entity->entity_ID);
	}
}

std::vector<std::shared_ptr<Event>> EntityManager::GetEventsEntities()
{
	std::vector<std::shared_ptr<Event>> events;

	for (auto entity : entities)
	{
		auto event = std::dynamic_pointer_cast<Event>(entity);

		if (event && event->type == EntityType::EVENT)
		{
			events.push_back(event);
		}
	}

	return events;
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

//std::shared_ptr<Entity> EntityManager::GetEnemy(int id) {
//	for (auto& entity : entities) {
//		auto enemy = std::dynamic_pointer_cast<BaseEnemy>(entity);
//		if (enemy && enemy->ID == id)
//			return enemy;
//	}
//	return nullptr;
//}
//

std::shared_ptr<BaseNPC> EntityManager::GetNPC(int id) {
	for (auto& entity : entities) {
		auto npc = std::dynamic_pointer_cast<BaseNPC>(entity);
		if (npc && npc->ID == id && npc->type == EntityType::BASENPC)
			return npc;
	}
	return nullptr;
}

std::shared_ptr<Entity> EntityManager::GetEntity_Map(int id, EntityType type)
{
	switch (type)
	{
	case EntityType::BASEENEMY:
		for (auto& entity : entities) {
			auto enemy = std::dynamic_pointer_cast<BaseEnemy>(entity);
			if (enemy && enemy->entity_ID == id)
				return enemy;
		}
		return nullptr;
		break;

	case EntityType::BASENPC:
		for (auto& entity : entities) {
			auto enemy = std::dynamic_pointer_cast<BaseNPC>(entity);
			if (enemy && enemy->entity_ID== id)
				return enemy;
		}
		return nullptr;
		break;

	case EntityType::BASECOMPANION:
		for (auto& entity : entities) {
			auto enemy = std::dynamic_pointer_cast<BaseCompanion>(entity);
			if (enemy && enemy->entity_ID == id)
				return enemy;
		}
		return nullptr;
		break;

	case EntityType::EVENT:
		for (auto& entity : entities) {
			auto enemy = std::dynamic_pointer_cast<Event>(entity);
			if (enemy && enemy->entity_ID == id)
				return enemy;
		}
		return nullptr;
		break;
	}
	
	return std::shared_ptr<Entity>();
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

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_U)) {
		LOG("Entities vector size: %i", entities.size());
	}

	return ret;
}