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

	cajonTex = Engine::GetInstance().textures->Load("Assets/Textures/cajon_Items.png");
	if (inventory->empty()) { 
		LoadItems();
	}
	return true;
}

bool ItemManager::Update(float dt)
{
	return true;

}

bool ItemManager::PostUpdate()
{
	if (showingPlayersItem)
	{
		Engine::GetInstance().render->DrawTexture(cajonTex,WindowSize.getX() - 200,WindowSize.getY() - 150);
		ShowBack();
	}

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
		ShowPlayerItems();
		break;
	case 2:
		break;
	case 3:
		if (showingPlayersItem) {
			!showingPlayersItem;
			Engine::GetInstance().render->CleanUp();
		}
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

	SDL_Rect bt2Pos = { WindowSize.getX() / 10, WindowSize.getY() / 3, 200,150 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, "Stats", bt2Pos, this));

	return true;
}

void ItemManager::ShowBack()
{
	SDL_Rect bt3Pos = { WindowSize.getX() - 200, WindowSize.getY() - 100, 135,68 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, "Back", bt3Pos, this));
}

bool ItemManager::ShowPlayerItems() {
	showingPlayersItem = true;
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

bool ItemManager::IsItemActive(const char* name)
{
	for (auto item : *inventory)
	{
		if (item.name == name)
		{
			return item.active;
		}
	}
}

void ItemManager::ApplyCombatItems(int &dmg_inc, int &shield_inc, int &confused_inc)
{
	for (auto item : *inventory)
	{
		if (item.name == "Cursed Knife" && item.active) dmg_inc = item.value;
		if (item.name == "Bike helmet" && item.active) shield_inc = item.value;
		if (item.name == "Gigantic flashlight" && item.active) confused_inc = item.value;
	}
}

void ItemManager::CreateButton(SDL_Texture* btnTex, SDL_Texture* btnPressedTex, SDL_Rect btPos, int n)
{
	auto btn = std::dynamic_pointer_cast<UIButton>(
		Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, n, " ", btPos, this));
	if (btn) btn->SetTextures(btnTex, btnPressedTex, btnPressedTex);
}
