#include "ItemManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Player.h"

ItemManager::ItemManager() : Module() { name = "ItemManager"; }

ItemManager::~ItemManager() {}

bool ItemManager::Awake() 
{
	inventory = new std::vector<Item>;
	return true;
}

bool ItemManager::Start()
{
	if (inventory->empty()) LoadItems();
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

bool ItemManager::LoadItemsData(std::string path, std::string fileName)
{
	itemsFileName = fileName;
	itemsPath = path;
	std::string mapPathName = itemsPath + itemsFileName;

	pugi::xml_parse_result result = itemsFileXML.load_file(mapPathName.c_str());
	if (result == NULL)
	{
		LOG("Could not load items xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("ItemsData.xml loaded successfully.");
	return true;
}

void ItemManager::LoadItems()
{
	for (pugi::xml_node item_tree_node = itemsFileXML.child("combat").child("items").child("item");
		item_tree_node != NULL;
		item_tree_node = item_tree_node.next_sibling("item"))
	{
		Item item;
		item.active = item_tree_node.attribute("active").as_bool();
		item.name = (const char*)item_tree_node.attribute("name").as_string();
		if (item_tree_node.attribute("value")) { item.value = item_tree_node.attribute("value").as_int(); }
		inventory->push_back(item);
	}
}

std::vector<Item>* ItemManager::GetItems()
{
	return inventory;
}
//show the items when pressing the "Items" button when in combat and when clicking a key.
void ItemManager::ShowInventory()
{
	img = Engine::GetInstance().textures->Load("Assets/Items/Item__67-export.png");
	Vector2D WindowSize = { (float)Engine::GetInstance().render->camera.w / 2,
							(float)Engine::GetInstance().render->camera.h / 2
	};
	Engine::GetInstance().render->DrawTexture(img, WindowSize.getX(), WindowSize.getY());
	
}

void ItemManager::UnShowInventory()
{
	if (img != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(img);
		img = nullptr;
	}
}

//bool ItemManager::IsItemActive(const char* name)
//{
//	for (Item item : *items)
//	{
//		if (item.name == name && item.active) return true;
//	}
//	return false;
//}

void ItemManager::AddItemToInventory(Item item) {
	
	inventory->push_back(item);
}

void ItemManager::ActivateItem(const char* name) {}

void ItemManager::ApplyCombatItems(int &dmg_inc, int &shield_inc, int &confused_inc)
{
	for (auto item : *inventory)
	{
		if (item.name == "Cursed Knife" && item.active) dmg_inc = item.value;
		if (item.name == "Bike helmet" && item.active) shield_inc = item.value;
		if (item.name == "Disturbing picture" && item.active) confused_inc = item.value;
	}
}
