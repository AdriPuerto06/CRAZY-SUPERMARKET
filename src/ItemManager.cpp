#include "ItemManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "CombatManager.h"

ItemManager::ItemManager() : Module() { name = "ItemManager"; }

ItemManager::~ItemManager() {}

bool ItemManager::Awake() 
{
	return true;
}

bool ItemManager::Start()
{
	return true;
}

bool ItemManager::Update(float dt)
{
	return true;
}

bool ItemManager::PostUpdate()
{
	return true;
}

bool ItemManager::CleanUp()
{
	return true;
}