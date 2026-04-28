#pragma once

#include "Entity.h"
#include <vector>
#include <SDL3/SDL.h>

struct SDL_Texture;

struct Item {
	std::string name;
	bool active;
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

	void ShowItems();
	void HideItems();

public:


private:
	
};
