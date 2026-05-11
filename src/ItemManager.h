#pragma once

#include "Entity.h"
#include <vector>
#include <SDL3/SDL.h>

struct SDL_Texture;

struct Item {
	const char* name;
	bool active;
	int value;
	std::string description;
	SDL_Texture* texture = nullptr;
};

class ItemManager : public Module
{
public:

	ItemManager();
	virtual ~ItemManager();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool OnUIMouseClickEvent(UIElement* uiElement);
	bool ShowInventoryOptions();
	bool ShowPlayerItems();
	bool ShowPlayerStats();
	void ShowBack();
	void CreateButton(SDL_Texture* btnOptTex, SDL_Texture* btnOptPressedTex, SDL_Rect btPos, int n);
	bool LoadItemsData(std::string path, std::string fileName);
	void LoadItems();
	std::vector<Item>* GetItems();

	void ShowItems();

	void ShowInventory();
	void UnShowInventory();

	void HideItems();

	void ActivateItem(const char* name);

	/*bool IsItemActive(const char* name);*/
	void ApplyCombatItems(int& dmg_inc, int& shield_inc, int& confused_inc);

	void AddItemToInventory(Item item);

public:

	SDL_Texture* img = nullptr;

private:
	Vector2D WindowSize;

	std::string itemsFileName;
	std::string itemsPath;
	pugi::xml_document itemsFileXML;
	bool showingPlayersItem = false;

	//inventory of the player
	std::vector<Item>* inventory;
	SDL_Texture* cajonTex = nullptr;

	bool showingBack = false;

};
