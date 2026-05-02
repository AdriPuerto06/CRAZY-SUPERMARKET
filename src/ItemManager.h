#pragma once

#include "Entity.h"
#include <vector>
#include <SDL3/SDL.h>

struct SDL_Texture;

struct Item {
	const char* name;
	bool active;
	int value;
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

	bool LoadItemsData(std::string path, std::string fileName);
	void LoadItems();
	std::vector<Item>* GetItems();

	void ShowItems();
	void HideItems();

	/*bool IsItemActive(const char* name);*/
	void ApplyCombatItems(int& dmg_inc, int& shield_inc, int& confused_inc);

public:


private:
	std::string itemsFileName;
	std::string itemsPath;
	pugi::xml_document itemsFileXML;
	std::vector<Item>* items;

};
