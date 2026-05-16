#include "ItemManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Player.h"
#include "UIManager.h"
#include "CombatManager.h"
#include "QuestManager.h"
#include "EntityManager.h"
#include <string>

ItemManager::ItemManager() : Module() { name = "ItemManager"; }

ItemManager::~ItemManager() {}

bool ItemManager::Awake() 
{
	inventory = new std::vector<Item>;
	return true;
}

bool ItemManager::Start()
{
	WindowSize = { (float)Engine::GetInstance().render->camera.w,
				   (float)Engine::GetInstance().render->camera.h };
	if (inventory->empty()) LoadItems();

	HPTex = Engine::GetInstance().textures->Load("");;
	MPTex = Engine::GetInstance().textures->Load("");;
	Atck1Tex = Engine::GetInstance().textures->Load("");;
	Atck2Tex = Engine::GetInstance().textures->Load("");;
	Atck3Tex = Engine::GetInstance().textures->Load("");;
	Atck4Tex = Engine::GetInstance().textures->Load("");;
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
	for (pugi::xml_node item_tree_node = itemsFileXML.child("items").child("item");
		item_tree_node != NULL;
		item_tree_node = item_tree_node.next_sibling("item"))
	{
		Item item;
		item.active = item_tree_node.attribute("active").as_bool();
		item.name = (const char*)item_tree_node.attribute("name").as_string();
		if (item_tree_node.attribute("value")) { item.value = item_tree_node.attribute("value").as_int(); }
		item.description = item_tree_node.attribute("description").as_string();
		item.texturePath = (const char*)item_tree_node.attribute("texturePath").as_string();
		item.texture = Engine::GetInstance().textures->Load(item.texturePath);
		

		AddItemToInventory(item);

		LOG("item : %s Loaded", item.name);
	}

	
}

bool ItemManager::OnUIMouseClickEvent(UIElement* uiElement)
{

	switch (uiElement->id)
	{
	case 1:
		Engine::GetInstance().scene->sceneStack.push(Engine::GetInstance().scene->GetCurrentScene());
		Engine::GetInstance().scene->ChangeScene(SceneID::ITEM);
		break;
	case 2:
		Engine::GetInstance().scene->sceneStack.push(Engine::GetInstance().scene->GetCurrentScene());
		Engine::GetInstance().scene->ChangeScene(SceneID::STATS);
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	default:
		break;
	}

	return true;
}

bool ItemManager::ShowInventoryOptions()
{
	LOG("ShowInventoryOptions called");
	//UnloadItemUI();

	SDL_Rect bt1Pos = { WindowSize.getX() / 10, WindowSize.getY() / 10, 200,150 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Items", bt1Pos, this));

	SDL_Rect bt2Pos = { WindowSize.getX() / 10, WindowSize.getY() / 10 + 200, 200,150 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, "Stats", bt2Pos, this));

	return true;
}

bool ItemManager::ShowingQuests()
{
	Engine::GetInstance().questManager->ViewQuest();

	return true;
}

bool ItemManager::ShowPlayerItems() {
	LOG("Item Butons Created");
	SDL_Rect bt1Pos = { WindowSize.getX() / 3, WindowSize.getY() / 5, 64,64};
	CreateButton(NULL, NULL, bt1Pos, NULL);
	SDL_Rect bt2Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 5, 64,64 };
	CreateButton(NULL, NULL, bt2Pos, NULL);
	SDL_Rect bt3Pos = { WindowSize.getX() / 3, WindowSize.getY() / 3 + 15, 64,64 };
	CreateButton(NULL, NULL, bt3Pos, NULL);
	SDL_Rect bt4Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 3 + 15, 64,64 };
	CreateButton(NULL, NULL, bt4Pos, NULL);
	SDL_Rect bt5Pos = { WindowSize.getX() / 3, WindowSize.getY() / 2 + 20, 64,64 };
	CreateButton(NULL, NULL, bt5Pos, NULL);
	SDL_Rect bt6Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 2 + 20, 64,64 };
	CreateButton(NULL, NULL, bt6Pos, NULL);
	return true;
}

bool ItemManager::ShowPlayerStats() {
	LOG("Stats Showed");
	auto player = Engine::GetInstance().entityManager->GetPlayerEM();
	std::string text;
	text = std::to_string(player->HP);

	SDL_Rect HPPos = { WindowSize.getX() / 3, WindowSize.getY() / 5, 64,64 };
	auto btn = std::dynamic_pointer_cast<UIButton>(
		Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, NULL, text.c_str(), HPPos, this));
	if (btn) btn->SetTextures(NULL, NULL, NULL);
	Engine::GetInstance().render->DrawTexture(HPTex, HPPos.x, HPPos.y);
	SDL_Rect MPPos = { WindowSize.getX() / 3, WindowSize.getY() / 3, 64,64 };
	Engine::GetInstance().render->DrawTexture(MPTex, MPPos.x, MPPos.y);

	//attacks
	SDL_Rect Atck1Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 5, 64,64 };
	Engine::GetInstance().render->DrawTexture(Atck1Tex, Atck1Pos.x, Atck1Pos.y);
	SDL_Rect Atck2Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 3, 64,64 };
	Engine::GetInstance().render->DrawTexture(Atck2Tex, Atck2Pos.x, Atck2Pos.y);
	SDL_Rect Atck3Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 2, 64,64 };
	Engine::GetInstance().render->DrawTexture(Atck3Tex, Atck3Pos.x, Atck3Pos.y);
	SDL_Rect Atck4Pos = { WindowSize.getX() / 3 + 300, WindowSize.getY() / 1.3f, 64,64 };
	Engine::GetInstance().render->DrawTexture(Atck4Tex, Atck4Pos.x, Atck4Pos.y);

	return true;
}

std::vector<Item>* ItemManager::GetItems()
{
	return inventory;
}
//show the items when pressing the "Items" button when in combat and when clicking a key.
void ItemManager::ShowInventory()
{
	int xincrement = 0;
	int yincrement = 0;
	for (int i = 0; i < inventory->size(); i++) {

		
		int posx = (WindowSize.getX() / 3) + xincrement;
		int posy = (WindowSize.getY() / 3) + yincrement;



		if (inventory->at(i).texture != nullptr) {
			Engine::GetInstance().render->DrawTexture(inventory->at(i).texture, posx, posy);
			Engine::GetInstance().render->DrawText(inventory->at(i).name, posx, posy + inventory->at(i).texture->h, 64, 32, { 0,0,0,0 });
		}
		else {
			Engine::GetInstance().render->DrawRectangle({ 32, 32 }, 225, 0, 0);
		}
		

		xincrement += 128;

	}

	
	Engine::GetInstance().render->DrawTexture(img, WindowSize.getX(), WindowSize.getY());
	
}

void ItemManager::UnShowInventory()
{
	
}

bool ItemManager::IsItemActive(const char* name)
{
	for (Item item : *inventory)
	{
		if (item.name == name) return item.active;
	}
}

void ItemManager::AddItemToInventory(Item item) {
	
	inventory->push_back(item);
}

void ItemManager::ActivateItem(const char* name) 
{
	for (auto item : *inventory)
	{
		if (item.name == name) 
		{ 
			if (item.active) { LOG("ItemManager: Item '%s' was already active.", name); }
			item.active = true;
			LOG("ItemManager: Item '%s' was already active.", name);
			/*Save items file*/ 
		}
	}
}

void ItemManager::ApplyCombatItems(int &dmg_inc, int &shield_inc, int &confused_inc)
{
	for (auto item : *inventory)
	{
		if (item.name == "Cursed Knife" && item.active) dmg_inc = item.value;
		if (item.name == "Bike helmet" && item.active) shield_inc = item.value;
		if (item.name == "Disturbing picture" && item.active) confused_inc = item.value;
	}
}

void ItemManager::CreateButton(SDL_Texture* btnTex, SDL_Texture* btnPressedTex, SDL_Rect btPos, int ID)
{
	auto btn = std::dynamic_pointer_cast<UIButton>(
		Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, ID, " ", btPos, this));
	if (btn) btn->SetTextures(btnTex, btnPressedTex, btnPressedTex);
}
